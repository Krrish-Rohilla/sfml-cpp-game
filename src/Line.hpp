#pragma once
#include <SFMl/Graphics.hpp>

class Line{
private:
    sf::Vector2f pointA;
    sf::Vector2f pointB;

    sf::Vector2f Direction;

    // using normal vector here to make calculations simple
    sf::Vector2f Normal;

    float thickness;

    sf::Color color;

    bool collisionFree; // NEW: Flag to skip physical interaction

public:
    // constructor using start point and end point of line
    Line(sf::Vector2f a, 
         sf::Vector2f b, 
         sf::Color color = sf::Color::White, 
         float thickness = 10.f) noexcept;


    // Fast inline getters for our future physics engine and batch renderer
    sf::Vector2f getA() const noexcept { return pointA; }
    sf::Vector2f getB() const noexcept { return pointB; }
    sf::Vector2f getDirection() const noexcept { return Direction; }
    sf::Vector2f getNormal() const noexcept { return Normal; }
    float getThickness() const noexcept { return thickness; }
    sf::Color getColor() const noexcept { return color; }

    // Setters in case you want to modify individual segments later
    void setColor(sf::Color c) noexcept { color = c; }
    void setThickness(float t) noexcept { thickness = t; }

    // NEW: Getters and setters for our upcoming spring lines
    bool isCollisionFree() const noexcept { return collisionFree; }
    void setCollisionFree(bool free) noexcept { collisionFree = free; }

};