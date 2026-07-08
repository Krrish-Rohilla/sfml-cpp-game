#include "Ball.hpp"
#include <cmath>

Ball::Ball(sf::Vector2f start, sf::Vector2f vel, sf::Color color, float r) noexcept {
    x = start.x;    y = start.y;
    vx = vel.x;     vy = vel.y;
    radius = r;
    shape.setRadius(radius);
    shape.setFillColor(color);
}

void Ball::updatePhysics(unsigned int h, float gravity, float friction, float e) noexcept {
    x += vx;
    y += vy;
    
    // Look-ahead state split physics logic
    if (y + vy >= (static_cast<float>(h) - radius * 2)) {
        vy *= -e;
        vx *= 1.f - friction;

        if (std::abs(vx) < 0.01f) vx = 0.f;
        if (std::abs(vy) < 0.01f) {
            vy = 0.f;
            y = static_cast<float>(h) - radius * 2;
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