#include "Ball.hpp"
#include <cmath>

Ball::Ball(float start_x, float start_y, float start_vx, float start_vy, sf::Color color) noexcept {
    x = start_x;
    y = start_y;
    vx = start_vx;
    vy = start_vy;
    shape.setRadius(50.f);
    shape.setFillColor(color);
}

void Ball::updatePhysics(unsigned int h, float gravity, float friction, float e) noexcept {
    x += vx;
    y += vy;
    
    // Look-ahead state split physics logic
    if (y + vy >= (static_cast<float>(h) - 100.f)) {
        vy *= -e;
        vx *= 1.f - friction;

        if (std::abs(vx) < 0.00001f) vx = 0.f;
        if (std::abs(vy) < 0.00001f) {
            vy = 0.f;
            y = static_cast<float>(h) - 100.f;
        }
    }
    else {
        vy += gravity;
    }

    shape.setPosition({x, y});
}

void Ball::draw(sf::RenderWindow& window) noexcept {
    window.draw(shape);
}