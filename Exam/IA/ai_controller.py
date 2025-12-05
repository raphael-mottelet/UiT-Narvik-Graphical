#!/usr/bin/env python3
"""
IA/ai_controller.py

Contrôleur IA pour le jeu de billes, à appeler depuis l'app C++ via un
sous-processus (popen). Utilise NumPy pour faire des calculs vectoriels.

PROTOCOLE D'ÉCHANGE (simple et texte, via stdin/stdout) :

Entrée (stdin) :
    Ligne 1 : N              (nombre de billes)
    Lignes 2..N+1 :
        is_player x y z vx vy vz radius
        - is_player : 0 ou 1
        - x,y,z     : position actuelle
        - vx,vy,vz  : vitesse actuelle
        - radius    : rayon de la bille
    Ligne N+2 :
        width startZ finishZ
    Ligne N+3 :
        dt          (delta time pour ce tick physique)

Sortie (stdout) :
    N lignes :
        ax az
    -> accélération suggérée (dans le plan XZ) pour chaque bille.
       Pour le joueur, on renvoie (0, 0) : le contrôle reste côté C++.

Exemple d’appel côté C++ (pseudo-code):

    FILE* pipe = _popen("python IA/ai_controller.py", "w+");
    fprintf(pipe, "%d\n", N);
    for (... chaque bille ...) {
        fprintf(pipe, "%d %f %f %f %f %f %f %f\n",
                is_player, x, y, z, vx, vy, vz, radius);
    }
    fprintf(pipe, "%f %f %f\n", width, startZ, finishZ);
    fprintf(pipe, "%f\n", dt);
    fflush(pipe);
    // puis lire N lignes depuis pipe, avec ax az
    _pclose(pipe);

Tu peux adapter ce protocole selon tes besoins.
"""

import sys
from dataclasses import dataclass
from typing import List, Tuple, TextIO
import numpy as np


@dataclass
class MarbleState:
    is_player: bool
    pos: np.ndarray  # shape (3,) -> [x, y, z]
    vel: np.ndarray  # shape (3,) -> [vx, vy, vz]
    radius: float


@dataclass
class TrackState:
    width: float
    start_z: float
    finish_z: float


# ---------------------------
# IA principale
# ---------------------------

def compute_ai_controls(
    marbles: List[MarbleState],
    track: TrackState,
    dt: float
) -> np.ndarray:
    """
    Calcule, pour chaque bille, un vecteur d'accélération [ax, az]
    dans le plan horizontal XZ.

    - Les billes du joueur (is_player == True) reçoivent [0, 0]
    - Les billes IA reçoivent une accélération basique :
        * suivent une ligne (lane) en X
        * avancent vers -Z (direction de la piste)
        * paramètres différents pour donner des "personnalités"
    """
    n = len(marbles)
    accels = np.zeros((n, 2), dtype=np.float32)  # [ax, az] pour chaque bille

    if n == 0:
        return accels

    # Lignes possibles (X) sur la piste, par exemple 3 lanes
    lanes_x = np.array([-1.0, 0.0, 1.0], dtype=np.float32)

    for i, m in enumerate(marbles):
        if m.is_player:
            # On ne touche pas au joueur ; C++ garde le contrôle
            accels[i, :] = 0.0
            continue

        # Choix d'une lane en fonction de l'index, pour les répartir
        lane_index = i % len(lanes_x)
        target_x = lanes_x[lane_index]

        # Steering latéral (vers target_x)
        dx = target_x - m.pos[0]
        steer_strength = 6.0  # force latérale
        ax = dx * steer_strength

        # Vitesse de base vers l'avant (–Z)
        base_speed = 2.5 + 0.5 * lane_index       # un peu différente par lane
        if track.finish_z < track.start_z:
            # Si la bille est proche de la ligne d'arrivée, on pousse un peu plus
            dist_to_finish = m.pos[2] - track.finish_z  # > 0
            boost = np.clip(30.0 / max(dist_to_finish, 1.0), 0.0, 2.0)
            target_speed = base_speed * (1.0 + 0.3 * boost)
        else:
            target_speed = base_speed

        # Vitesse actuelle vers -Z : on veut vel_z ~ -target_speed
        current_speed_forward = -m.vel[2]
        accel_z_gain = 4.0
        delta_speed = target_speed - current_speed_forward
        forward_accel = delta_speed * accel_z_gain  # vers -Z

        az = -forward_accel  # parce que aller vers -Z => az négatif

        accels[i, 0] = ax
        accels[i, 1] = az

    return accels


# ---------------------------
# Parsing / IO
# ---------------------------

def parse_input(stdin: TextIO) -> Tuple[List[MarbleState], TrackState, float]:
    """
    Lit l'état complet depuis stdin selon le protocole décrit en haut.
    """
    lines = [line.strip() for line in stdin if line.strip() != ""]
    if not lines:
        raise ValueError("Entrée vide pour l'IA Python")

    it = iter(lines)
    try:
        n = int(next(it))
    except StopIteration:
        raise ValueError("Entrée incomplète (manque N)")

    marbles: List[MarbleState] = []
    for _ in range(n):
        try:
            parts = next(it).split()
        except StopIteration:
            raise ValueError("Entrée incomplète (manque une ligne de bille)")

        if len(parts) != 8:
            raise ValueError(f"Ligne bille invalide : {parts}")

        is_player = bool(int(parts[0]))
        x, y, z, vx, vy, vz, radius = map(float, parts[1:])

        marbles.append(
            MarbleState(
                is_player=is_player,
                pos=np.array([x, y, z], dtype=np.float32),
                vel=np.array([vx, vy, vz], dtype=np.float32),
                radius=radius,
            )
        )

    try:
        parts = next(it).split()
        if len(parts) != 3:
            raise ValueError("Ligne track invalide (width startZ finishZ)")
        width, start_z, finish_z = map(float, parts)
    except StopIteration:
        raise ValueError("Entrée incomplète (manque la ligne de track)")

    track = TrackState(width=width, start_z=start_z, finish_z=finish_z)

    try:
        dt = float(next(it))
    except StopIteration:
        raise ValueError("Entrée incomplète (manque dt)")

    return marbles, track, dt


def main() -> int:
    try:
        marbles, track, dt = parse_input(sys.stdin)
        accels = compute_ai_controls(marbles, track, dt)
    except Exception as e:
        # En cas d'erreur, on écrit sur stderr et on retourne un code d'erreur
        sys.stderr.write(f"[ai_controller] ERROR: {e}\n")
        sys.stderr.flush()
        return 1

    # Sortie : N lignes "ax az"
    for ax, az in accels:
        sys.stdout.write(f"{ax:.6f} {az:.6f}\n")

    sys.stdout.flush()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
