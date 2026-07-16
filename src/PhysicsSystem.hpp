#pragma once
#include "RigidBody.hpp"
#include <vector>
#include <memory>
#include <utility>

// Forward declarations to keep compilation speeds lightning fast and interfaces clean
class Spring;
class SpatialGrid;

/**
 * @brief The core physics pipeline. 
 * Systematically runs integration, spring updates, Spatial Grid queries, 
 * Separating Axis Theorem (SAT) overlap checks, and positional impulse corrections.
 */
class PhysicsSystem {
public:
    /**
     * @brief Executes the complete 4-pass sequential physics execution pipeline loop.
     * @param bodies Master data store vector containing all active scene rigid entities.
     * @param springs Numerical compliance constraint string lattices.
     * @param grid Broadphase uniform grouping acceleration grid wrapper.
     * @param dt Scaled simulation timestep delta value (seconds).
     * @param gravity Constant acceleration tracking downward field.
     * @param airDrag Macro kinetic environment friction damping factor.
     */
    static void step(std::vector<std::unique_ptr<RigidBody>>& bodies,
                     std::vector<std::unique_ptr<Spring>>& springs,
                     SpatialGrid& grid,
                     float dt,
                     float gravity,
                     float airDrag) noexcept;

private:
    /**
     * @brief Industry-standard manifold structure mapping geometric point contact telemetry.
     */
    struct ContactManifold {
        RigidBody* bodyA;
        RigidBody* bodyB;
        sf::Vector2f normal;     // Minimum penetration intersection axis vector
        float penetrationDepth;  // Overlap distance magnitude along the collision axis
    };

    // --- The Sequential Physics Pipeline Components (Passes 1-4) ---
    
    // Pass 1: Integration Pass - Updates velocity frames via linear/rotational forces.
    static void integrateForces(std::vector<std::unique_ptr<RigidBody>>& bodies, float dt, float gravity, float airDrag) noexcept;
    
    // Pass 2: Constraint Pass - Evaluates Hooke's Law equations across linked spring bodies.
    static void evaluateConstraints(std::vector<std::unique_ptr<Spring>>& springs, float dt) noexcept;
    
    // Pass 3: Broadphase Filter - Remaps uniform cells and queries close neighbor pairs.
    static void performBroadphase(std::vector<std::unique_ptr<RigidBody>>& bodies, 
                                  SpatialGrid& grid, 
                                  std::vector<std::pair<RigidBody*, RigidBody*>>& potentialPairs) noexcept;
    
    // Pass 4: Narrowphase SAT - Executes deep collision overlapping projections.
    static void performNarrowphase(std::vector<std::pair<RigidBody*, RigidBody*>>& potentialPairs, 
                                   std::vector<ContactManifold>& contactPool) noexcept;
    
    // Post-Pass: Impulse Resolution Solver - Applies physical reaction torque and linear push-backs.
    static void resolveCollisions(std::vector<ContactManifold>& contactPool, float dt) noexcept;

    // --- Foundational Separating Axis Theorem (SAT) Geometric Checkers ---
    static bool testCircleVsCircle(RigidBody* a, RigidBody* b, ContactManifold& manifold) noexcept;
    static bool testPolygonVsPolygon(RigidBody* a, RigidBody* b, ContactManifold& manifold) noexcept;
    static bool testCircleVsPolygon(RigidBody* circle, RigidBody* polygon, ContactManifold& manifold) noexcept;
};