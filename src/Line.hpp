#pragma once
#include <SFMl/Graphics.hpp>

class Line{
private:
    sf::Vector2f pointA;
    sf::Vector2f pointB;

    sf::Vector2f Direction;

    // using normal vector here to make calculations simple
    sf::Vector2f Normal;

    // defining a container of points
    sf::VertexArray va;

    float thickness;

public:
    // constructor using start point and end point of line
    Line(sf::Vector2f a, sf::Vector2f b, sf::Color color, float thick) noexcept;

    void draw(sf::RenderWindow& window) noexcept;

    sf::Vector2f getA() {return pointA;}

    sf::Vector2f getB() {return pointB;}

    sf::Vector2f getNormal() {return Normal;}

};