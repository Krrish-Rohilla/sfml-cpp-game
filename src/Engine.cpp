#include "Engine.hpp"
#include <SFML/Window/Event.hpp>
#include "MapParser.hpp"

Engine::Engine(const std::string& windowTitle, unsigned int width, unsigned int height) noexcept
    : window(sf::VideoMode({width, height}), windowTitle), // SFML 3 unified braced Vector initializer conversion
      levelManager(981.0f, 0.01f, 128.0f)                  // Inject foundational baseline configuration metrics
{
    // Restricting framerate prevents hardware coil whine and limits extreme idle thread loops
    window.setFramerateLimit(144);

    // Load the default startup map instead of spawning objects manually
    // This maintains modularity: the Engine boots, then loads the scene data.
    MapParser::loadMap("assets/maps/default.txt", levelManager, mapRenderer);
}

void Engine::run() noexcept {
    coreClock.restart();

    while (window.isOpen()) {
        // High-precision delta time extraction per tick
        const float dt = coreClock.restart().asSeconds();

        handleEvents();
        update(dt);
        render();
    }
}

void Engine::handleEvents() noexcept {
    // C++26 Optimized Flow: Using std::optional event evaluation directly within the condition loop
    while (const std::optional event = window.pollEvent()) {
        
        // Modern SFML 3 Type-Safe Checks: Replaces the old switch(event.type) union structure
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return;
        }

        // De-reference the optional layer safely and forward the concrete event down the execution chain
        levelManager.processInput(*event);
    }
}

void Engine::update(float dt) noexcept {
    levelManager.update(dt);
}

void Engine::render() noexcept {
    // Clear graphic buffers using a clean dark canvas
    window.clear(sf::Color(15, 15, 18));

    // Route the active render context down to catalog visible shape primitives
    levelManager.render(window);

    // Swap back-buffers to display completed geometry passes
    window.display();
}