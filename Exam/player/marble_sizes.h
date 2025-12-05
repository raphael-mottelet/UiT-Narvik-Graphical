#pragma once

#include "../utils/state.h"

// Simple type for three marble sizes
enum class MarbleSizeKind
{
    Small  = 0,
    Medium = 1,
    Large  = 2
};

inline float marbleRadiusFor(MarbleSizeKind kind)
{
    // Maps marble size choice to physical radius used for physics and rendering; smaller radius gives agile, fast marble, larger radius gives heavier, slower marble and changes collision footprint
    switch (kind)
    {
    case MarbleSizeKind::Small:  return 0.35f; // small and fast
    case MarbleSizeKind::Large:  return 0.80f; // big and slow
    case MarbleSizeKind::Medium:
    default:                     return 0.50f; // default size
    }
}


// Simple mass model: mass proportional to volume (radius^3)
inline float marbleMassFor(MarbleSizeKind kind)
{
    float r = marbleRadiusFor(kind);
    return r * r * r;
}

// Change the player marble size (radius + mass)
inline void setPlayerMarbleSize(AppState& state, MarbleSizeKind kind)
{
    Marble* player = getPlayerMarble(state);
    if (!player)
        return;

    player->radius = marbleRadiusFor(kind);
    player->mass   = marbleMassFor(kind);
}
