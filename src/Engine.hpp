#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Ball.hpp"
#include "Line.hpp"
#include "PhysicsSystem.hpp"
#include "MapGenerator.hpp"

class Engine {
private:
    sf::RenderWindow window;
    unsigned int w, h;
    
    float gravity;
    float e;
    float groundFriction;
    float airFriction;

    std::vector<Ball> balls;
    std::vector<Line> lines;
    sf::VertexArray trackMesh;  // NEW: Holds the single global visual track model

    sf::Texture basketballTexture; // NEW: Holds the texture file image buffer

    void processEvents() noexcept;
    void update() noexcept;
    void render() noexcept;

public:
    Engine();
    void run() noexcept;
};