#pragma once
#include <string>
#include <string_view>

class LevelManager;
class MapRenderer;

/**
 * @brief Utility for loading map definitions from disk and orchestrating 
 * the instantiation of physics and visual systems.
 */
class MapParser {
public:
    MapParser() = delete; // Utility class only

    /**
     * @brief Parses a map file and populates both the physics and rendering pipelines.
     * @param filepath Path to the map data file.
     * @param manager Reference to LevelManager for physics entity spawning.
     * @param renderer Reference to MapRenderer for building the visual vertex cache.
     */
    [[nodiscard]] static bool loadMap(const std::string& filepath, 
                                      LevelManager& manager, 
                                      MapRenderer& renderer) noexcept;
};