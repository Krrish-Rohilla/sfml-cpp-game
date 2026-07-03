#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Ball.hpp"

class Engine {
private:
    sf::RenderWindow window;
    unsigned int w, h;
    
    float gravity;
    float e;
    float friction;

    std::vector<Ball> balls;

    void processEvents() noexcept;
    void update() noexcept;
    void render() noexcept;

public:
    Engine();
    void run() noexcept;
};