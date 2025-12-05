#pragma once
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "state.h"

// Appelle le script Python IA/ai_controller.py via des fichiers temporaires.
// Protocole aligné avec ton ai_controller.py :
//
//  Entrée (IA/ai_input.txt) :
//    Ligne 1 : N
//    Lignes 2..N+1 : is_player x y z vx vy vz radius
//    Ligne N+2 : width startZ finishZ
//    Ligne N+3 : dt
//
//  Sortie (IA/ai_output.txt) :
//    N lignes : ax az
//
// On applique ensuite ces accélérations aux billes IA uniquement.
inline void applyAIFromPython(AppState& state, float dt){
    if (state.marbles.empty()){
        return;
    }

    // 1) Écrire l'état courant dans IA/ai_input.txt
    std::ofstream out("IA/ai_input.txt");
    if (!out.is_open()){
        static bool warned = false;
        if (!warned){
            std::cerr << "[AI] Impossible d'ouvrir IA/ai_input.txt en écriture\n";
            warned = true;
        }
        return;
    }

    const std::size_t N = state.marbles.size();
    out << N << "\n";

    for (const Marble& m : state.marbles){
        out << (m.isPlayer ? 1 : 0) << " "
            << m.position.x << " " << m.position.y << " " << m.position.z << " "
            << m.velocity.x << " " << m.velocity.y << " " << m.velocity.z << " "
            << m.radius << "\n";
    }

    // Ligne de track : width startZ finishZ
    const Track& track = state.track;
    out << track.width << " " << track.startZ << " " << track.finishZ << "\n";

    // Ligne de dt
    out << dt << "\n";
    out.close();

    // 2) Appeler le script Python
    //    Si ta commande est "py" au lieu de "python", adapte la chaîne ci-dessous.
    int code = std::system("python IA/ai_controller.py < IA/ai_input.txt > IA/ai_output.txt");
    if (code != 0){
        static bool warned = false;
        if (!warned){
            std::cerr << "[AI] python IA/ai_controller.py a échoué (code "
                      << code << "). IA désactivée.\n";
            warned = true;
        }
        return;
    }

    // 3) Lire les accélérations calculées
    std::ifstream in("IA/ai_output.txt");
    if (!in.is_open()){
        static bool warned = false;
        if (!warned){
            std::cerr << "[AI] Impossible d'ouvrir IA/ai_output.txt en lecture\n";
            warned = true;
        }
        return;
    }

    for (std::size_t i = 0; i < N; ++i){
        float ax = 0.0f, az = 0.0f;
        if (!(in >> ax >> az)){
            break;
        }

        Marble& m = state.marbles[i];
        if (m.isPlayer){
            // On ne touche pas au joueur : les inputs clavier restent maîtres
            continue;
        }

        // v = v + a * dt
        m.velocity.x += ax * dt;
        m.velocity.z += az * dt;
    }
}
