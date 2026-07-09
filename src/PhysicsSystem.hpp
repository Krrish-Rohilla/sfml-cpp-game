#pragma once
#include "Ball.hpp"
#include "Line.hpp"
#include <vector>

class PhysicsSystem {
public:
    // It loops through all balls and checks them against all line segments
    static void resolveCollisions(std::vector<Ball>& balls, const std::vector<Line>& lines, float e, float groundFriction) noexcept;

private:
    // A helper function to handle the math for one specific ball hitting one specific line.
    // Making it static and private keeps it safely hidden inside this system's logic box.
    static void checkBallLineCollision(Ball& ball, const Line& line, float e, float groundFriction) noexcept;
};