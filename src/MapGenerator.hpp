#pragma once
#include <SFML/Graphics.hpp>
#include "Line.hpp"
#include <vector>

class MapGenerator {
public:
    // using static to use function without making a object
    // Arguments: lines, radius, {x,y}, angle, color, thickness, segments
    static void addCurve (std::vector<Line>& lines, float radius, sf::Vector2f center, float angle, sf::Color color, float thickness, int segments, float start);

    // our actual map
    static void buildPark (std::vector<Line>& lines);
};