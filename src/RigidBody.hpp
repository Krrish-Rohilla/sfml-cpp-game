#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

/**
 * @brief Categorizes the geometric classification for Narrowphase SAT routing.
 */
enum class ShapeType {
    Circle,
    Polygon
};

/**
 * @brief Unified physics object definition replacing custom isolated entities.
 * Represents Circles, Polygons, Obstacles, and Static Walls through mass, 
 * restitution, friction coefficients, and inertia tensors. Includes triggers for specialized zones.
 */
class RigidBody {
private:
    ShapeType shapeType;             

    // Kinematic State Framework
    sf::Vector2f position;           // Center of mass coordinates[cite: 5]
    sf::Vector2f velocity;           // Linear velocity vector[cite: 5]
    sf::Vector2f forceAccumulator;   // Integrated external forces (e.g., Gravity/Wind)[cite: 5]
    
    float orientationAngle;          // Rotational orientation tracking in radians[cite: 5]
    float angularVelocity;           // Angular velocity (positive = CCW, negative = CW)[cite: 5]
    float torqueAccumulator;         // Rotational force accumulation framework[cite: 5]

    // Mass & Inertial Tensor Inversions
    float mass;                      // Absolute mass value[cite: 5]
    float inverseMass;               // 0.f explicitly flags Static Walls / Infinite Mass Obstacles[cite: 5]
    float momentOfInertia;           // Mass distribution scalar factor[cite: 5]
    float inverseInertia;            // 0.f explicitly locks rotational stability for static bodies[cite: 5]

    // Material Mechanical Coefficients
    float restitution;               // Elastic bounce coefficient (e)[cite: 5]
    float friction;                  // Tangent surface kinetic friction coefficient[cite: 5]

    // Bounding Footprints & Geometric Geometry Arrays
    float radius;                            // Circle radius or outer boundary scalar for Spatial Grid queries[cite: 5]
    std::vector<sf::Vector2f> localVertices; // Centered local mesh vertex definitions for SAT projections[cite: 5]

    // Environmental Field System Interconnection
    bool triggerFlag;                // isTrigger flag to bypass structural collision impulses[cite: 5]

    // Lifecycle & Presentation Metadata Flags
    bool deadFlag{false};            // Triggers deferred deletion removal passes within the LevelManager
    bool visibleFlag{true};          // Controls viewport rendering pipeline execution paths[cite: 3]
    std::string assetTag{""};        // Maps the data profile to localized texture assets without coupling dependencies

public:
    // Explicit constructor configuration optimized for Circular Entities[cite: 5]
    explicit RigidBody(sf::Vector2f spawnPosition, 
                       float circleRadius, 
                       sf::Vector2f initialVelocity, 
                       float bodyMass,
                       float bodyRestitution,
                       float bodyFriction,
                       bool isTriggerZone = false) noexcept;

    // Explicit constructor configuration optimized for Convex Polygonal Entities / Extruded Lines[cite: 5]
    explicit RigidBody(sf::Vector2f spawnPosition,
                       const std::vector<sf::Vector2f>& vertices,
                       sf::Vector2f initialVelocity,
                       float bodyMass,
                       float bodyRestitution,
                       float bodyFriction,
                       bool isTriggerZone = false) noexcept;

    // Core kinematic linear/rotational update mechanics[cite: 5]
    void updatePhysics(float airDragCoefficient) noexcept;
    
    // External Field Integrators[cite: 5]
    void addForce(sf::Vector2f force) noexcept;
    void addTorque(float torque) noexcept;
    void clearForces() noexcept;

    // Production-Grade Clean C++26 Inline Accessors & Mutators[cite: 5]
    [[nodiscard]] ShapeType getType() const noexcept { return shapeType; }
    
    [[nodiscard]] sf::Vector2f getPosition() const noexcept { return position; }
    void setPosition(sf::Vector2f pos) noexcept;
    
    [[nodiscard]] sf::Vector2f getVelocity() const noexcept { return velocity; }
    void setVelocity(sf::Vector2f vel) noexcept { velocity = vel; }
    
    [[nodiscard]] float getRotationAngle() const noexcept { return orientationAngle; }
    void setRotationAngle(float radians) noexcept;
    
    [[nodiscard]] float getAngularVelocity() const noexcept { return angularVelocity; }
    void setAngularVelocity(float w) noexcept { angularVelocity = w; }
    
    [[nodiscard]] float getRadius() const noexcept { return radius; }
    [[nodiscard]] const std::vector<sf::Vector2f>& getLocalVertices() const noexcept { return localVertices; }
    
    [[nodiscard]] float getMass() const noexcept { return mass; }
    [[nodiscard]] float getInverseMass() const noexcept { return inverseMass; }
    [[nodiscard]] float getInverseInertia() const noexcept { return inverseInertia; }
    [[nodiscard]] float getRestitution() const noexcept { return restitution; }
    [[nodiscard]] float getFriction() const noexcept { return friction; }

    [[nodiscard]] bool isTrigger() const noexcept { return triggerFlag; }
    void setTrigger(bool state) noexcept { triggerFlag = state; }

    // --- Lifecycle Interface Operations ---
    [[nodiscard]] bool isDead() const noexcept { return deadFlag; } // Permanently resolves LevelManager compilation drop issues[cite: 3]
    void setDead(bool state) noexcept { deadFlag = state; }

    // --- Presentation Interface Operations ---
    [[nodiscard]] bool isVisible() const noexcept { return visibleFlag; }
    void setVisible(bool state) noexcept { visibleFlag = state; }

    [[nodiscard]] const std::string& getAssetTag() const noexcept { return assetTag; }
    void setAssetTag(const std::string& tag) noexcept { assetTag = tag; }
};