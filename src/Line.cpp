#include "Line.hpp"
#include "VectorMath.hpp"
#include <cmath>

Line::Line(sf::Vector2f a, sf::Vector2f b, sf::Color color, float thickness) noexcept {
    pointA = a;
    pointB = b;
    this->thickness = thickness;
    this->color = color; // Save the custom color for the batch renderer to read later

    Direction = Math::normalize(pointB - pointA);

    Normal = Math::Normal(Direction);

    this->collisionFree = false; // NEW: Default to normal solid lines

}