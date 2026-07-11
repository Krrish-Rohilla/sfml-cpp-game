#pragma once
#include "Ball.hpp"
#include "Line.hpp"
#include <vector>

class PhysicsSystem {
public:
    // Global entry point to compile constraints and run the engine's LCP solver passes
    static void resolveCollisions(std::vector<Ball>& balls, const std::vector<Line>& lines, float e, float groundFriction) noexcept;

private:
    // Clean structural blueprint matching industry standards (e.g., Box2D / PhysX)
    struct Contact {
        Ball* ball;
        const Line* line;
        sf::Vector2f normal;
        sf::Vector2f tangent;
        sf::Vector2f closestPoint;
        float normalImpulse;   // Total accumulated normal impulse over solver iterations
        float tangentImpulse;  // Total accumulated friction impulse over solver iterations
        float targetVelocity;  // Calculated physical bounce target velocity
    };
};