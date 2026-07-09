#pragma once
#include <SFML/Graphics.hpp>

class Ball {
private:
    sf::CircleShape shape;
    float x, y;
    float vx, vy;
    float radius;
    float mass;

public:
    Ball(sf::Vector2f start, float radius = 50.f, sf::Color color = sf::Color::Cyan, sf::Vector2f vel = {0.f, 0.f}, float mass = 1.f) noexcept;

    void updatePhysics(unsigned int h, float gravity, float friction, float e) noexcept;
    void draw(sf::RenderWindow& window) noexcept;

    // Getters & Setters so you can change values later
    sf::Vector2f getPosition() const noexcept { return {x, y}; }
    sf::Vector2f getVelocity() const noexcept { return {vx, vy}; }
    float getRadius() const noexcept { return radius; }
    float getMass() const noexcept { return mass; }

    void setPosition(sf::Vector2f pos) noexcept {
        x = pos.x;
        y = pos.y;
        shape.setPosition({x, y});
    }

    void setVelocity(sf::Vector2f vel) noexcept {
        vx = vel.x;
        vy = vel.y;
    }

    void setFillColor(sf::Color color){
        shape.setFillColor(color);
    }

    void setMass(float m) noexcept {
        mass = m;
    }
};