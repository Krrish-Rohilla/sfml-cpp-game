#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <algorithm>

namespace Math {
    // 1. Returns the scalar length (magnitude) of a vector
    [[nodiscard]] inline float length(const sf::Vector2f& v) noexcept {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    // 2. Returns the squared length (avoids square root calculation for fast comparisons)
    [[nodiscard]] constexpr float lengthSq(const sf::Vector2f& v) noexcept {
        return v.x * v.x + v.y * v.y;
    }

    // 3. Returns the Euclidean distance between two points
    [[nodiscard]] inline float distance(const sf::Vector2f& v1, const sf::Vector2f& v2) noexcept {
        return length(v1 - v2);
    }

    // 4. Returns the squared distance between two points
    [[nodiscard]] constexpr float distanceSq(const sf::Vector2f& v1, const sf::Vector2f& v2) noexcept {
        return lengthSq(v1 - v2);
    }

    // 5. Returns a normalized (unit length) copy of the vector; returns zero vector if length is near zero
    [[nodiscard]] inline sf::Vector2f normalize(const sf::Vector2f& v) noexcept {
        const float lenSq = lengthSq(v);
        if (lenSq > 1e-8f) {
            const float invLen = 1.0f / std::sqrt(lenSq);
            return v * invLen;
        }
        return {0.0f, 0.0f};
    }

    // 6. Returns a normalized vector with a custom fallback if vector length is negligible
    [[nodiscard]] inline sf::Vector2f safeNormalize(const sf::Vector2f& v, const sf::Vector2f& fallback = {0.0f, 0.0f}) noexcept {
        const float lenSq = lengthSq(v);
        if (lenSq > 1e-8f) {
            const float invLen = 1.0f / std::sqrt(lenSq);
            return v * invLen;
        }
        return fallback;
    }

    // 7. Returns the dot product of two vectors
    [[nodiscard]] constexpr float dot(const sf::Vector2f& v1, const sf::Vector2f& v2) noexcept {
        return v1.x * v2.x + v1.y * v2.y;
    }

    // 8. 2D Cross Product (Vector x Vector) -> Scalar representing magnitude & orientation
    [[nodiscard]] constexpr float cross(const sf::Vector2f& v1, const sf::Vector2f& v2) noexcept {
        return v1.x * v2.y - v1.y * v2.x;
    }

    // 9. 2D Cross Product (Vector x Scalar) -> Perpendicular Vector scaled
    [[nodiscard]] constexpr sf::Vector2f cross(const sf::Vector2f& v, float s) noexcept {
        return {s * v.y, -s * v.x};
    }

    // 10. 2D Cross Product (Scalar x Vector) -> Perpendicular Vector scaled
    [[nodiscard]] constexpr sf::Vector2f cross(float s, const sf::Vector2f& v) noexcept {
        return {-s * v.y, s * v.x};
    }

    // 11. Returns a 90-degree counter-clockwise perpendicular vector
    [[nodiscard]] constexpr sf::Vector2f perpendicular(const sf::Vector2f& v) noexcept {
        return {-v.y, v.x};
    }

    // 12. Vector projection of v onto target vector
    [[nodiscard]] inline sf::Vector2f project(const sf::Vector2f& v, const sf::Vector2f& target) noexcept {
        const float targetLenSq = lengthSq(target);
        if (targetLenSq < 1e-8f) return {0.0f, 0.0f};
        return target * (dot(v, target) / targetLenSq);
    }

    // 13. Reflects vector v off a surface defined by unit normal vector
    [[nodiscard]] constexpr sf::Vector2f reflect(const sf::Vector2f& v, const sf::Vector2f& normal) noexcept {
        return v - 2.0f * dot(v, normal) * normal;
    }

    // 14. Clamps vector magnitude to a maximum value
    [[nodiscard]] inline sf::Vector2f clampLength(const sf::Vector2f& v, float maxLength) noexcept {
        const float lenSq = lengthSq(v);
        if (lenSq > maxLength * maxLength && lenSq > 1e-8f) {
            return v * (maxLength / std::sqrt(lenSq));
        }
        return v;
    }

    // 15. Linear interpolation between two vectors
    [[nodiscard]] constexpr sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t) noexcept {
        return a + t * (b - a);
    }

    // 16. Rotates a 2D vector by a given angle in radians
    [[nodiscard]] inline sf::Vector2f rotate(const sf::Vector2f& v, float angleRadians) noexcept {
        const float cosA = std::cos(angleRadians);
        const float sinA = std::sin(angleRadians);
        return { v.x * cosA - v.y * sinA, v.x * sinA + v.y * cosA };
    }

    // 17. Returns the polar angle of a vector in radians (-PI to PI)
    [[nodiscard]] inline float angle(const sf::Vector2f& v) noexcept {
        return std::atan2(v.y, v.x);
    }

    // 18. Returns the signed angle from v1 to v2 in radians
    [[nodiscard]] inline float angleBetween(const sf::Vector2f& v1, const sf::Vector2f& v2) noexcept {
        return std::atan2(cross(v1, v2), dot(v1, v2));
    }

    // 19. Returns a 90-degree clockwise perpendicular vector
    [[nodiscard]] constexpr sf::Vector2f perpendicularCW(const sf::Vector2f& v) noexcept {
        return {v.y, -v.x};
    }

    // 20. Normalized linear interpolation for smooth direction transitions
    [[nodiscard]] inline sf::Vector2f nlerp(const sf::Vector2f& a, const sf::Vector2f& b, float t) noexcept {
        return normalize(lerp(a, b, t));
    }
}
