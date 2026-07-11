#pragma once
#include <SFML/Graphics.hpp>

class Ball {
private:
    sf::CircleShape shape;
    float x, y; // Tracks the TRUE CENTER of mass
    float vx, vy;
    float radius;
    float mass;
    float omega = 0.f; // NEW: Angular velocity (positive = CCW, negative = CW)
    float angle = 0.f; // Tracks current rotation orientation in radians

public:
    Ball(sf::Vector2f start, float radius = 50.f, sf::Color color = sf::Color::Cyan, sf::Vector2f vel = {0.f, 0.f}, float mass = 1.f) noexcept;

    void updatePhysics(unsigned int h, float gravity, float airDrag, float e) noexcept;
    void draw(sf::RenderWindow& window) noexcept;

    // Standard Getters & Setters returning the true mathematical center
    sf::Vector2f getPosition() const noexcept { return {x, y}; }
    sf::Vector2f getVelocity() const noexcept { return {vx, vy}; }
    float getRadius() const noexcept { return radius; }
    float getMass() const noexcept { return mass; }

    // NEW: Getters and setters for rotation
    float getAngularVelocity() const noexcept { return omega; }
    void setAngularVelocity(float w) noexcept { omega = w; }

    // NEW: Get and set orientation angles using native SFML 3 Angle handles
    float getRotationAngle() const noexcept { return angle; }
    void setRotationAngle(float radians) noexcept {
        angle = radians;
        shape.setRotation(sf::radians(angle)); // Automatically converts to native SFML units
    }

    // NEW: Asset binder that overrides vector colors with an image texture
    void setTexture(const sf::Texture& texture) noexcept {
        shape.setTexture(&texture);
        shape.setFillColor(sf::Color::White); // Wipes out previous base tint so texture looks natural
    }

    void setPosition(sf::Vector2f pos) noexcept {
        x = pos.x;
        y = pos.y;
        // Automatically offset visual asset so it renders perfectly centered
        shape.setPosition({x, y});
    }

    void setVelocity(sf::Vector2f vel) noexcept {
        vx = vel.x;
        vy = vel.y;
    }

    void setFillColor(sf::Color color) {
        shape.setFillColor(color);
    }
};