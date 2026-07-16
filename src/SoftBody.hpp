#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>

class RigidBody;
struct Spring;
class LevelManager;

/**
 * @brief High-level controller representing a cohesive deformable soft body entity.
 * Rather than managing heap memory directly, this class acts as a lightweight observer container
 * tracking non-owning pointers to particles (RigidBodies) and structural links (Springs)
 * owned and processed by the LevelManager.
 */
class SoftBody {
public:
    // --- Construction / Destruction ---
    SoftBody() noexcept = default;
    ~SoftBody() noexcept = default;

    // Rule of 5: Prevent resource copying to guarantee pointer integrity, allow moves safely
    SoftBody(const SoftBody&) = delete;
    SoftBody& operator=(const SoftBody&) = delete;
    SoftBody(SoftBody&&) noexcept = default;
    SoftBody& operator=(SoftBody&&) noexcept = default;

    // --- Static Factory Spawners ---
    
    /**
     * @brief Spawns a cohesive, pressure-filled circular blob.
     * Creates a perimeter of mass points connected by outer springs, reinforced with
     * internal cross-springs to maintain shape volume under compression.
     */
    static SoftBody createBlob(LevelManager& manager, 
                               sf::Vector2f center, 
                               float radius, 
                               float particleRadius, 
                               std::size_t pointCount, 
                               float totalMass, 
                               float stiffness, 
                               float damping) noexcept;

    /**
     * @brief Spawns a chain of linked particles forming a rope or cable.
     * Creates successive connected segments, making it ideal for hanging bridges, whips, or swing ropes.
     */
    static SoftBody createRope(LevelManager& manager, 
                               sf::Vector2f start, 
                               sf::Vector2f end, 
                               std::size_t segmentCount, 
                               float particleRadius, 
                               float totalMass, 
                               float stiffness, 
                               float damping) noexcept;

    /**
     * @brief Spawns a 2D sheet of particles cross-braced with structural, shear, and bending springs.
     * Creates a cloth-like physics mesh or a deformable soft-box.
     */
    static SoftBody createGrid(LevelManager& manager, 
                               sf::Vector2f topLeft, 
                               float width, 
                               float height, 
                               std::size_t columns, 
                               std::size_t rows, 
                               float particleRadius, 
                               float totalMass, 
                               float stiffness, 
                               float damping) noexcept;

    // --- Entity Manipulation Interfaces ---
    
    /**
     * @brief Applies an external linear force vector to all constituent particles simultaneously.
     */
    void applyForce(sf::Vector2f force) noexcept;

    /**
     * @brief Instantly sets the linear velocity of all constituent particles.
     */
    void setVelocity(sf::Vector2f velocity) noexcept;

    /**
     * @brief Toggles the rendering visibility of the soft body's nodes and springs.
     */
    void setVisible(bool visible) noexcept;

    /**
     * @brief Flags all constituent particles as dead, scheduling them for deferred deletion.
     */
    void destroy() noexcept;

    // --- Core Utility Accessors ---
    
    [[nodiscard]] sf::Vector2f getCenterOfMass() const noexcept;
    [[nodiscard]] const std::vector<RigidBody*>& getParticles() const noexcept { return particles; }
    [[nodiscard]] const std::vector<Spring*>& getSprings() const noexcept { return constraints; }

private:
    std::vector<RigidBody*> particles;
    std::vector<Spring*> constraints;
};