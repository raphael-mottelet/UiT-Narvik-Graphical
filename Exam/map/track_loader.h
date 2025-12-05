// map/track_loader.h
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "../utils/state.h"

// Trim helper that removes leading and trailing whitespace characters from a string
inline std::string trim(const std::string& s)
{
    auto first = std::find_if_not(s.begin(), s.end(), ::isspace);
    if (first == s.end())
        return {};

    auto last = std::find_if_not(s.rbegin(), s.rend(), ::isspace).base();
    return std::string(first, last);
}

// Simple hard-coded fallback track used when no map file is available; track dimensions and obstacles are later consumed by physics and by the OpenGL rasterization-based renderer
inline void initDefaultTrack(Track& track)
{
    track.width   = 5.0f;    // Track width in world units along X used by lateral physics constraints and track rendering in rasterization
    track.startZ  = 0.0f;    // Start line position along Z used as initial spawn and visual start marker in the rasterized scene
    track.finishZ = -40.0f;  // Finish line position along Z used for winner detection and drawing the end of the track
    track.obstacles.clear();

    // Demo obstacle near the beginning of the race used as a physical blocker and rendered box in the rasterized scene
    {
        Obstacle o;
        o.center = glm::vec3(0.0f, 0.0f, -10.0f); // Obstacle center in world space affecting both collision checks and visual placement
        o.size   = glm::vec2(1.2f, 1.5f);         // Obstacle half-size in X and Z controlling collision footprint and rasterized box size
        track.obstacles.push_back(o);
    }
    // Demo obstacle on the left lane further down the track
    {
        Obstacle o;
        o.center = glm::vec3(-1.5f, 0.0f, -20.0f);
        o.size   = glm::vec2(1.0f, 2.0f);
        track.obstacles.push_back(o);
    }
    // Demo obstacle on the right lane near the end of the default track
    {
        Obstacle o;
        o.center = glm::vec3(1.5f, 0.0f, -30.0f);
        o.size   = glm::vec2(1.0f, 2.0f);
        track.obstacles.push_back(o);
    }
}

// Load a numeric map file that defines track width, start and finish Z positions, and rectangular obstacles used by marble physics and by the rasterized track rendering
inline bool tryLoadTrackFromFile(const std::string& path, Track& track)
{
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cout << "[track_loader] cannot open file: " << path << "\n";
        return false;
    }

    std::cout << "[track_loader] loading: " << path << "\n";

    // Default parameters in case the file omits some values so physics and rasterization always have a valid track
    track.width   = 5.0f;
    track.startZ  = 0.0f;
    track.finishZ = -40.0f;
    track.obstacles.clear();

    std::string line;
    int         lineNumber = 0;

    while (std::getline(in, line)) {
        ++lineNumber;
        line = trim(line);
        if (line.empty())
            continue;
        if (line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "WIDTH") {
            float w;
            if (iss >> w) {
                track.width = w; // Width parameter used by lateral movement constraints and by track mesh size in rasterization
            }
        }
        else if (token == "START_Z") {
            float z;
            if (iss >> z) {
                track.startZ = z; // Start position along Z that defines spawn area and visible start line in the rasterized scene
            }
        }
        else if (token == "FINISH_Z") {
            float z;
            if (iss >> z) {
                track.finishZ = z; // Finish position along Z that defines where winner detection happens and where the track visually ends
            }
        }
        else if (token == "OBSTACLE") {
            float cx, cz, sx, sz;
            if (!(iss >> cx >> cz >> sx >> sz)) {
                std::cout << "[track_loader] bad OBSTACLE line " << lineNumber
                          << " in " << path << "\n";
                continue;
            }

            Obstacle o;
            // Obstacle center in world space with y = 0 so physics collision tests match the ground plane and the rasterized obstacle appears on the track surface
            o.center = glm::vec3(cx, 0.0f, cz);

            // Obstacle size as half-extent in X and Z used both for marble-obstacle collision volumes and for the width and depth of the rasterized box
            o.size   = glm::vec2(sx, sz);

            track.obstacles.push_back(o);
        }
        else {
            std::cout << "[track_loader] unknown token '" << token
                      << "' at line " << lineNumber
                      << " in " << path << "\n";
        }
    }

    std::cout << "[track_loader] loaded " << track.obstacles.size()
              << " obstacles, width=" << track.width
              << ", startZ=" << track.startZ
              << ", finishZ=" << track.finishZ << "\n";

    return true;
}

// Fallback initializer that tries several candidate map paths and uses a built-in track when no file can be loaded so physics and rasterized rendering always have valid track data
inline void initTrackFallback(AppState& state)
{
    if (tryLoadTrackFromFile("map/track_complex.txt", state.track)) return;
    if (tryLoadTrackFromFile("map/01-map.txt",       state.track)) return;
    if (tryLoadTrackFromFile("map/02-map.txt",       state.track)) return;
    if (tryLoadTrackFromFile("track.txt",            state.track)) return;

    std::cout << "[track_loader] no map file found, using default track\n";
    initDefaultTrack(state.track);
}
