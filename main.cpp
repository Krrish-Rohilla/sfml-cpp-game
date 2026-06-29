#include <SFML/Graphics.hpp>
#include <iostream>
int main() {
    // Defining the Screen Resolution
    sf::VideoMode mymode ({800,600});
    // Defining the Title on Title Bar
    const std::string title = "Welcome to SFML world";
    //Defining and Close and Resize Buttons
    std::uint32_t style = sf::Style::Close;
    // Floating Window or FullScreen State
    sf::State state = sf::State::Windowed;
    // Fining the Window Borders
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    // Creating the Window
    sf::RenderWindow window(mymode, title, style, state, settings);

    // Keeping Frame Rate 60fps
    window.setFramerateLimit(60);

    while(window.isOpen()){

        // Traking inputs from the User
        while (std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()){
                window.close(); // This closes the window
            }
        }
        window.clear(sf::Color(30,5,10)); // Setting the Background Colour
        window.display();                //Updating the Display
    }

    return 0;
}