#include "Line.hpp"
#include <cmath>

Line::Line(sf::Vector2f a, sf::Vector2f b, sf::Color color, float thick) noexcept {
    pointA = a;
    pointB = b;
    thickness = thick;

    sf::Vector2f direction = pointB - pointA;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    sf::Vector2f Direction = direction / length;

    sf::Vector2f Normal (-Direction.y, Direction.x);

    // Calculate the offset from the center line (half the thickness on each side)
    sf::Vector2f offset = Normal * (thickness / 2.0f);

    // using buildin feature to make lines easily
    va.setPrimitiveType(sf::PrimitiveType::TriangleStrip);

    // Append the 4 corners in a specific Z-order for the GPU Triangle Strip
    va.append(sf::Vertex(pointA + offset, color)); // Top-Left (Corner 0)
    va.append(sf::Vertex(pointA - offset, color)); // Bottom-Left (Corner 1)
    va.append(sf::Vertex(pointB + offset, color)); // Top-Right (Corner 2)
    va.append(sf::Vertex(pointB - offset, color)); // Bottom-Right (Corner 3)
}

void Line::draw(sf::RenderWindow& window) noexcept {
    window.draw(va);
}