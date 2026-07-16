#pragma once

// Forward declaration isolates compilation boundaries to maximize compile speeds
class RigidBody;

/**
 * @brief A high-performance linear spring-damper constraint link.
 * Connects two arbitrary RigidBody instances to evaluate Hooke's Law with linear damping.
 * Multiple springs cross-linked between body arrays form stable softbody mass-spring lattices.
 */
struct Spring {
    // --- Default Constructor (REQUIRED for make_unique) ---
    Spring() noexcept = default;
    // --- Authoritative Entity Connections ---
    RigidBody* bodyA{nullptr}; // Observer pointer to the first anchored physical entity matrix[cite: 1, 3]
    RigidBody* bodyB{nullptr}; // Observer pointer to the second anchored physical entity matrix[cite: 1, 3]

    // --- Core Mechanical Parameters ---
    float restLength{0.0f};    // The natural equilibrium distance of the spring in world units[cite: 1, 3]
    float stiffness{100.0f};   // Hooke's Law spring coefficient scalar (k), governing elasticity[cite: 1, 3]
    float damping{2.0f};       // Internal structural friction damping factor, preventing eternal oscillation[cite: 1, 3]

    // --- Visibility Architecture Constraints ---
    bool visible{true};        // If false, the physics loop processes the spring as an invisible joint/hinge[cite: 1, 3]

    // --- High-Performance Trivial Semantics ---
    // A constraint component container owns no heap memory directly; keeping it trivially 
    // copyable and movable allows optimal vector mutations and maximum CPU cache efficiency.
    ~Spring() noexcept = default;
    Spring(const Spring&) noexcept = default;
    Spring& operator=(const Spring&) noexcept = default;
    Spring(Spring&&) noexcept = default;
    Spring& operator=(Spring&&) noexcept = default;
};