#include "MapParser.hpp"
#include "LevelManager.hpp"
#include "MapRenderer.hpp"
#include <fstream>
#include <sstream>
#include <vector>

bool MapParser::loadMap(const std::string& filepath, LevelManager& manager, MapRenderer& renderer) noexcept {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    renderer.clear();
    std::string line;

    // Simple format: RECT x y w h
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip empty lines/comments

        std::istringstream iss(line);
        std::string type;
        float x, y, w, h;
        
        if (!(iss >> type >> x >> y >> w >> h)) continue;

        if (type == "RECT") {
            // 1. Spawn the Physics Collider
            const std::vector<sf::Vector2f> vertices = { {0, 0}, {w, 0}, {w, h}, {0, h} };
            manager.spawnPolygon({x, y}, vertices, {0, 0}, 0.0f, 0.2f, 0.5f, false);

            // 2. Build Visual Geometry for Batching
            sf::VertexArray rect(sf::PrimitiveType::Triangles, 6);
            
            // Simple 2-triangle construction for a rectangle
            rect[0] = {{x, y}, sf::Color::White};
            rect[1] = {{x + w, y}, sf::Color::White};
            rect[2] = {{x, y + h}, sf::Color::White};
            rect[3] = {{x + w, y}, sf::Color::White};
            rect[4] = {{x + w, y + h}, sf::Color::White};
            rect[5] = {{x, y + h}, sf::Color::White};

            renderer.addGeometry(rect);
        }
    }
    return true;
}