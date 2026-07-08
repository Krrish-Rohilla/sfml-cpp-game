#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Ball.hpp"
#include "Line.hpp"
#include "MapGenerator.hpp"

class Engine {
private:
    sf::RenderWindow window;
    unsigned int w, h;
    
    float gravity;
    float e;
    float friction;

    std::vector<Ball> balls;
    std::vector<Line> lines;

    void processEvents() noexcept;
    void update() noexcept;
    void render() noexcept;

public:
    Engine();
    void run() noexcept;
};