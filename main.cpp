#include <SFML/Graphics.hpp>
#include <iostream>
#include<cmath>
int main() {
    unsigned width = 800, height = 600;
    // Defining the Screen Resolution
    sf::VideoMode mymode ({width, height});
    // Defining the Title on Title Bar
    const std::string title = "Welcome to SFML world";
    //Defining and Close and Resize Buttons
    std::uint32_t style = sf::Style::Close;
    // Floating Window or FullScreen State
    sf::State state = sf::State::Windowed;
    // Fining the Window Borders
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    // Defining a ball
    sf::CircleShape ball(50.f);
    ball.setFillColor(sf::Color::Blue);

    // Introducing gravity
    float gravity = 0.4;

    // Coefficient of restitution
    float e = 0.71;

    // friction
    float friction = 0.01;

    // ball position
    float ball_x = 0, ball_y = 0;
    // ball velocity
    float ballv_x = 2, ballv_y = 0;

    // Creating the Window
    sf::RenderWindow window(mymode, title, style, state, settings);

    // Keeping Frame Rate 60fps
    window.setFramerateLimit(60);

    while(window.isOpen()){

        ball_x += ballv_x;
        ball_y += ballv_y;
        
        if (ball_y >= 500.f){
            ballv_y *= -1.f * e;
            ballv_x = ballv_x > 0? ballv_x - friction: 0;

            ball.setPosition({ball_x, height-100.f});

            if (std::abs(ballv_y) < 0.00001){
                ballv_y = 0;
                ball_y = height - 100.f;
            }
        }
        else {
            ballv_y += gravity;
            ball.setPosition({ball_x, ball_y});
        }


        // Traking inputs from the User
        while (std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()){
                window.close(); // This closes the window
            }
            if (event->is<sf::Event::KeyPressed>()){}
        }
        window.clear(sf::Color(20,0,3));        // Setting the Background Colour

        window.draw(ball);

        window.display();                       // Updating the Display
    }

    return 0;
}