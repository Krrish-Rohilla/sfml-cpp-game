#include "MapGenerator.hpp"
#include <cmath>

void MapGenerator::addCurve (std::vector<Line>& lines, float radius, sf::Vector2f center, float angle, sf::Color color, float thickness, int segments, float start = 0.f){

    std::vector<sf::Vector2f> curvePoints;

    float pi = std::numbers::pi;

    float totalRadians = (angle / 180.f) * -pi;

    // Generate points up to our target angle
    for (int i = 0; i <= segments; ++i) {
        float progress = (float)i / (float)segments;
        
        // Starts at a baseline angle (downward 90 deg) and transitions toward the target angle
        float currentAngle = (progress * totalRadians) - (start / 180) * pi;

        float x = center.x + radius * std::cos(currentAngle);
        float y = center.y + radius * std::sin(currentAngle);

        curvePoints.push_back({x, y});
    }

    // Connect the points together
    for (size_t i = 0; i < curvePoints.size() - 1; ++i) {
        lines.push_back(Line(curvePoints[i], curvePoints[i + 1], color, thickness));
    }

}

void MapGenerator::buildPark(std::vector<Line>& lines) {

// 1. Flat platform
lines.push_back(Line({0.f, 150.f}, {150.f, 150.f}, sf::Color::White, 10.f));

// 2. Beautiful curve using your exact parameter ordering layout!
// Arguments: lines, radius, {x,y}, angle, color, thickness, segments
addCurve(lines, 200.f, {350.f, 150.f}, -90.f, sf::Color::Green, 10.f, 20, -90.f);

// 3. Flat bottom floor connecting exactly where the curve finished
lines.push_back(Line({350.f, 350.f}, {600.f, 350.f}, sf::Color(255, 180, 0), 10.f));

}