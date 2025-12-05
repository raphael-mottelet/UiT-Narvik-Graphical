#pragma once

#include <string>
#include <vector>

// Simple description of a track that can be selected in the menu
struct MapInfo {
    std::string name;
    std::string path;
};

// Presets for the marble radius that the player can choose
enum class MarbleSize {
    Small  = 0,
    Medium = 1,
    Large  = 2
};

// Human-readable label for the size shown in the menu
inline const char* marbleSizeLabel(MarbleSize s) {
    switch (s) {
    case MarbleSize::Small:  return "Small";
    case MarbleSize::Medium: return "Medium";
    case MarbleSize::Large:  return "Large";
    default:                 return "Medium";
    }
}

// Actual radius used by the physics for each preset
inline float marbleSizeRadius(MarbleSize s) {
    switch (s) {
    case MarbleSize::Small:  return 0.30f;
    case MarbleSize::Medium: return 0.40f;
    case MarbleSize::Large:  return 0.55f;
    default:                 return 0.40f;
    }
}

// Runtime state for the menu overlay
struct MenuState {
    bool visible       = true;              // Menu shown when true
    bool justSelected  = false;             // True for one frame after ENTER
    int  selectedIndex = 0;                 // Index into maps
    MarbleSize marbleSize = MarbleSize::Medium;
    std::vector<MapInfo> maps;              // Available maps
};

// Fill menu with default values and a single example map
inline void initMenu(MenuState& menu) {
    menu.visible       = true;
    menu.justSelected  = false;
    menu.selectedIndex = 0;
    menu.marbleSize    = MarbleSize::Medium;
    menu.maps.clear();

    MapInfo m;
    m.name = "Complex track";
    m.path = "map/track_complex.txt";
    menu.maps.push_back(m);
}
