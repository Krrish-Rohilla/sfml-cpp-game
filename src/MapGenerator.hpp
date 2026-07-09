#pragma once
#include <SFML/Graphics.hpp>
#include "Line.hpp"
#include <vector>

class MapGenerator {
public:
    // using static to use function without making a object
    // Arguments: lines, radius, {x,y}, angle, color, thickness, segments
    static void addCurve (std::vector<Line>& lines, float radius, sf::Vector2f center, float angle, float start, sf::Color color, float thickness, int segments);

    // our actual map
    static void buildPark (std::vector<Line>& lines);

    // NEW: The baking assembly utility
    static void bakeVisualMesh(const std::vector<Line>& lines, sf::VertexArray& batch) noexcept;

};