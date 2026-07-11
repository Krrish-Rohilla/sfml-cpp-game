#include "Ball.hpp"
#include <cmath>

Ball::Ball(sf::Vector2f start, float radius, sf::Color color, sf::Vector2f vel, float mass) noexcept {
    this->radius = radius;
    this->mass = mass;
    this->omega = 0.f; 
    this->angle = 0.f; // NEW: Reset visual spin orientation on creation
    
    // Convert incoming top-left spawn coordinates to clean Center of Mass coordinates
    this->x = start.x + radius;    
    this->y = start.y + radius;
    this->vx = vel.x;     
    this->vy = vel.y;
    
    shape.setRadius(this->radius);
    shape.setFillColor(color);
    
    // NEW: Shift the rotation/translation pivot from the top-left (0,0) to geometric center
    shape.setOrigin({this->radius, this->radius});
    shape.setPosition({x, y});
}

void Ball::updatePhysics(unsigned int h, float gravity, float airDrag, float e) noexcept {
    vx *= (1.f - airDrag);
    vy *= (1.f - airDrag);
}

void Ball::draw(sf::RenderWindow& window) noexcept {
    window.draw(shape);
}