#pragma once
#include "RigidBody.hpp"
#include "PhysicsSystem.hpp"
#include "SpatialGrid.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <memory>

// Forward declaration of Spring to protect compile boundaries 
class Spring;

/**
 * @brief Authoritative state owner and lifecycle manager for the game scene.
 * Manages unique ownership buffers of entities, routes window input configurations, 
 * and implements a deterministic fixed-timestep accumulator loop for physics substepping.
 */
class LevelManager {
public:
    /**
     * @brief Configures the manager simulation parameters and initializes the broadphase grid.
     * @param gravity Vertical constant acceleration matrix magnitude.
     * @param airDrag Environmental velocity damping coefficients.
     * @param spatialCellSize Bounding dimension for broadphase uniform cell mapping.
     */
    explicit LevelManager(float gravity = 981.0f, float airDrag = 0.01f, float spatialCellSize = 128.0f) noexcept;

    // --- High-Performance System Rule of 5 ---
    ~LevelManager() noexcept;
    LevelManager(const LevelManager&) = delete;
    LevelManager& operator=(const LevelManager&) = delete;
    LevelManager(LevelManager&&) noexcept = default;
    LevelManager& operator=(LevelManager&&) noexcept = default;

    /**
     * @brief Intercepts raw engine events and routes them to active player controllers.
     * Prevents global state coupling by driving input downward through entity layers.
     * @param event The native SFML window frame input telemetry event object.
     */
    void processInput(const sf::Event& event) noexcept;

    /**
     * @brief Runs the fixed substepping time accumulator simulation step.
     * Splits variable frame times into uniform deterministic slices to execute the physics engine.
     * @param deltaTime The raw, un-smoothed elapsed frame time reported by the main engine clock.
     */
    void update(float deltaTime) noexcept;

    /**
     * @brief Dispatches the active entity list down to the SFML render pipeline.
     * @param window The target graphics frame rendering buffer window object.
     */
    void render(sf::RenderWindow& window) const noexcept;

    // --- Factory Spawning Interface Methods ---
    
    RigidBody* spawnCircle(sf::Vector2f position, float radius, sf::Vector2f velocity, 
                           float mass, float restitution, float friction, bool isTrigger) noexcept;

    RigidBody* spawnPolygon(sf::Vector2f position, const std::vector<sf::Vector2f>& vertices, 
                            sf::Vector2f velocity, float mass, float restitution, float friction, bool isTrigger) noexcept;

    void addSpring(RigidBody* bodyA, RigidBody* bodyB, float restLength, float stiffness, float damping) noexcept;

    /**
     * @brief Flushes the simulation level entirely, purging all springs and entities safely.
     */
    void clearLevel() noexcept;
    
    // --- Serialization Accessors ---
    [[nodiscard]] const std::vector<std::unique_ptr<RigidBody>>& getBodies() const noexcept { return bodies; }
    [[nodiscard]] const std::vector<std::unique_ptr<Spring>>& getSprings() const noexcept { return springs; }


private:
    /**
     * @brief Executes the "Poison Pill Pattern" for deferred deletion sweeps.
     * Safely purges flagged entities out of allocations at the end of the execution frame.
     */
    void postFrameCleanup() noexcept;

    // Master ownership memory buffers tracking active level state configurations
    std::vector<std::unique_ptr<RigidBody>> bodies;
    std::vector<std::unique_ptr<Spring>> springs;
    SpatialGrid spatialGrid;

    // Global environmental constants for calculation injection passes
    float gravity;
    float airDrag;

    // Substepping Time Accumulator Configuration Registers
    float timeAccumulator{ 0.0f };
    const float fixedTimeStep{ 1.0f / 60.0f }; // 60Hz highly stable physics tick limit
};