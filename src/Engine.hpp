#pragma once
#include "LevelManager.hpp"
#include "MapRenderer.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <string>

/**
 * @brief Authoritative application runtime container and core system orchestrator.
 * Governs the OS window context, manages the master high-precision hardware clock, 
 * handles top-level system events, and drives the central game frame loop.
 */
class Engine {
public:
    /**
     * @brief Initializes sub-systems and prepares the OS window container.
     * @param windowTitle The text display mapped to the OS window title bar.
     * @param width Desired horizontal window resolution in pixels.
     * @param height Desired vertical window resolution in pixels.
     */
    Engine(const std::string& windowTitle = "2D Custom Physics Engine", 
           unsigned int width = 1280, 
           unsigned int height = 720) noexcept;

    // --- Strict Resource Anchor: Non-Copyable & Non-Movable ---
    // Enforcing these deletions protects the hardware window context from 
    // accidental copying or memory migration bugs, securing a crash-free runtime.
    ~Engine() noexcept = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    /**
     * @brief Instantiates the execution frame loop.
     * Blocks execution thread and runs continuously until the application window context drops.
     */
    void run() noexcept;

    LevelManager& getLevelManager() noexcept { return levelManager; }

private:
    /**
     * @brief Polls native operating system events and forwards inputs down to managers.
     */
    void handleEvents() noexcept;

    /**
     * @brief Aggregates elapsed frame time and triggers deterministic state steps.
     * @param dt The precision floating-point delta time slice yielded by the hardware clock.
     */
    void update(float dt) noexcept;

    /**
     * @brief Manages the visual frame pipeline by clearing buffers and dispatching render ticks.
     */
    void render() noexcept;

    // --- Core Engine Hardware Registers ---
    sf::RenderWindow window;   // Definitive OS window context and graphics frame buffer tracker
    sf::Clock coreClock;       // High-precision hardware timer tracking elapsed tick intervals

    // --- Encapsulated System State Managers ---
    LevelManager levelManager; // Master coordinator governing entity vectors, grids, and substepping

    MapRenderer mapRenderer;
};