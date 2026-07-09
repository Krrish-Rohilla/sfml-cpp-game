#include "MapGenerator.hpp"
#include <cmath>


void MapGenerator::addCurve (std::vector<Line>& lines, float radius, sf::Vector2f center, float angle, float start = 0.f, sf::Color color = sf::Color::Red, float thickness = 10, int segments = 15){

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

//_____________________________________________________________________________________
void MapGenerator::buildPark(std::vector<Line>& lines) {

// 1. Flat platform
lines.push_back(Line({0.f, 150.f}, {150.f, 150.f}, sf::Color::White, 10.f));

// 2. Beautiful curve using your exact parameter ordering layout!
// Arguments: lines, radius, {x,y}, angle, color, thickness, segments
addCurve(lines, 200.f, {350.f, 150.f}, -90.f, -90.f);

// 3. Flat bottom floor connecting exactly where the curve finished
lines.push_back(Line({350.f, 350.f}, {600.f, 350.f}, sf::Color(255, 180, 0), 10.f));

lines.push_back(Line({0.f, 0.f}, {0.f, 600.f}));
lines.push_back(Line({0.f, 600.f}, {1000.f, 600.f}));
lines.push_back(Line({1000.f, 600.f}, {1000.f, 0.f}));
lines.push_back(Line({1000.f, 0.f}, {0.f, 0.f}));

}
//_____________________________________________________________________________________


void MapGenerator::bakeVisualMesh(const std::vector<Line>& lines, sf::VertexArray& batch) noexcept {
    batch.clear();
    batch.setPrimitiveType(sf::PrimitiveType::Triangles);
    // 8 slices = 8 triangles × 3 vertices = 24 vertices.
    batch.resize(lines.size() * 54);  // 54 vertices form 2 triangles + 2 circle

    std::size_t vertexIndex = 0;
    const float pi = std::numbers::pi;
    const int circleSegments = 8;   // Slices of the pizza

    for (const auto& line: lines){
        sf::Vector2f pA = line.getA();
        sf::Vector2f pB = line.getB();
        sf::Vector2f normal = line.getNormal();
        float radius = line.getThickness() / 2.f;
        sf::Color lineColor = line.getColor();

        // ─────────────────────────────────────────────────────────
        // 1. DRAW THE MAIN RECTANGLE TRACK (6 Vertices)
        // ─────────────────────────────────────────────────────────
        sf::Vector2f topLeft     = pA + normal * radius;
        sf::Vector2f bottomLeft  = pA - normal * radius;
        sf::Vector2f topRight    = pB + normal * radius;
        sf::Vector2f bottomRight = pB - normal * radius;

        batch[vertexIndex++] = sf::Vertex(topLeft,     lineColor);
        batch[vertexIndex++] = sf::Vertex(bottomLeft,  lineColor);
        batch[vertexIndex++] = sf::Vertex(topRight,    lineColor);

        batch[vertexIndex++] = sf::Vertex(bottomLeft,  lineColor);
        batch[vertexIndex++] = sf::Vertex(bottomRight, lineColor);
        batch[vertexIndex++] = sf::Vertex(topRight,    lineColor);

        // ─────────────────────────────────────────────────────────
        // 2. DRAW FULL CIRCLE AT POINT A (24 Vertices)
        // ─────────────────────────────────────────────────────────
        for (int i = 0; i < circleSegments; ++i) {
            float currentAngle = (static_cast<float>(i) / circleSegments) * 2.f * pi;
            float nextAngle    = (static_cast<float>(i + 1) / circleSegments) * 2.f * pi;

            // Center of pizza slice
            batch[vertexIndex++] = sf::Vertex(pA, lineColor);
            // Outer point 1
            batch[vertexIndex++] = sf::Vertex(pA + sf::Vector2f(std::cos(currentAngle) * radius, std::sin(currentAngle) * radius), lineColor);
            // Outer point 2
            batch[vertexIndex++] = sf::Vertex(pA + sf::Vector2f(std::cos(nextAngle) * radius, std::sin(nextAngle) * radius), lineColor);
        }

        // ─────────────────────────────────────────────────────────
        // 3. DRAW FULL CIRCLE AT POINT B (24 Vertices)
        // ─────────────────────────────────────────────────────────
        for (int i = 0; i < circleSegments; ++i) {
            float currentAngle = (static_cast<float>(i) / circleSegments) * 2.f * pi;
            float nextAngle    = (static_cast<float>(i + 1) / circleSegments) * 2.f * pi;

            // Center of pizza slice
            batch[vertexIndex++] = sf::Vertex(pB, lineColor);
            // Outer point 1
            batch[vertexIndex++] = sf::Vertex(pB + sf::Vector2f(std::cos(currentAngle) * radius, std::sin(currentAngle) * radius), lineColor);
            // Outer point 2
            batch[vertexIndex++] = sf::Vertex(pB + sf::Vector2f(std::cos(nextAngle) * radius, std::sin(nextAngle) * radius), lineColor);
        }
    }
}



