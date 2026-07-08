#pragma once
#include <SFML/Graphics.hpp>

class Ball {
private:
    sf::CircleShape shape;
    float x, y;
    float vx, vy;
    float radius;

public:
    Ball(sf::Vector2f start, sf::Vector2f vel, sf::Color color, float r) noexcept;

    void updatePhysics(unsigned int h, float gravity, float friction, float e) noexcept;
    void draw(sf::RenderWindow& window) noexcept;
};