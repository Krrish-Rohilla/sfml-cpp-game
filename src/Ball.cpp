#include "Ball.hpp"
#include <cmath>

Ball::Ball(sf::Vector2f start, float radius, sf::Color color, sf::Vector2f vel, float mass) noexcept {
    x = start.x;    
    y = start.y;
    vx = vel.x;     
    vy = vel.y;
    this->radius = radius; // Using this-> to distinguish parameter from class member
    this->mass = mass;     // Using this-> to distinguish parameter from class member
    
    shape.setRadius(this->radius);
    shape.setFillColor(color);
}

void Ball::updatePhysics(unsigned int h, float gravity, float airDrag, float e) noexcept {
    // 1. Standard Euler Integration (Movement)
    x += vx;
    y += vy;
    
    // 2. Continuous Gravity Acceleration
    vy += gravity;
    
    // 3. PURE AIR DRAG (Only drops energy if airDrag > 0.f)
    vx *= (1.f - airDrag);
    vy *= (1.f - airDrag);

    shape.setPosition({x, y});
}

void Ball::draw(sf::RenderWindow& window) noexcept {
    window.draw(shape);
}