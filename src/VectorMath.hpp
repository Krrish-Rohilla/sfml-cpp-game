#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace Math {
    // 1. Returns the scalar length (magnitude) of a vector
    [[nodiscard]] inline float length(const sf::Vector2f& v) noexcept {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    // 2. Returns the squared length (much faster, excellent for performance optimization)
    [[nodiscard]] inline float lengthSq(const sf::Vector2f& v) noexcept {
        return v.x * v.x + v.y * v.y;
    }

    // 3. Returns a normalized (unit length of 1.0) copy of the vector
    [[nodiscard]] inline sf::Vector2f normalize(const sf::Vector2f& v) noexcept {
        float len = length(v);
        if (len != 0.f) {
            return v / len;
        }
        return {0.f, 0.f};
    }

    // 4. Returns the dot product of two vectors (tells us how much they point in the same direction)
    [[nodiscard]] inline float dot(const sf::Vector2f& v1, const sf::Vector2f& v2) noexcept {
        return v1.x * v2.x + v1.y * v2.y;
    }

    // 5. Returns the 2D Cross Product scalar
    // In 2D, the cross product tells us if vector B is to the left (-) or right (+) of vector A
    [[nodiscard]] inline float cross(const sf::Vector2f& v1, const sf::Vector2f& v2) noexcept {
        return v1.x * v2.y - v1.y * v2.x;
    }

    // 6. Returns a vector rotated 90 degrees counter-clockwise (The Left-Hand Normal)
    [[nodiscard]] inline sf::Vector2f Normal(const sf::Vector2f& v) noexcept {
        return {-v.y, v.x};
    }
}