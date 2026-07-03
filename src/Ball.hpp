#pragma once
#include <SFML/Graphics.hpp>

class Ball {
private:
    sf::CircleShape shape;
    float x, y;
    float vx, vy;

public:
    Ball(float start_x, float start_y, float start_vx, float start_vy, sf::Color color) noexcept;

    void updatePhysics(unsigned int h, float gravity, float friction, float e) noexcept;
    void draw(sf::RenderWindow& window) noexcept;
};