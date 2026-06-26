#include <SFML/Graphics.hpp>

int main() {
    // SFML 3 uses sf::Vector2u for dimensions instead of separate numbers
    sf::RenderWindow window(sf::VideoMode({800, 600}), "My First SFML Game!");

    // Main Game Loop
    while (window.isOpen()) {
        // SFML 3 event handling uses std::optional
        while (const std::optional event = window.pollEvent()) {
            // Check if the window close button was clicked
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Clear the screen with a black color
        window.clear(sf::Color::Black);

        // Display whatever was drawn
        window.display();
    }

    return 0;
}