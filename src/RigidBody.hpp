#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <vector>
#include <string>

/**
 * @brief Categorizes physical mobility behavior.
 */
enum class BodyType {
    Dynamic,   // Fully simulated with mass and inertia
    Static,    // Infinite mass, unmovable level environment
    Kinematic  // Velocity-driven, unaffected by external forces/collisions
};

/**
 * @brief Categorizes geometric classification for Narrowphase SAT routing.
 */
enum class ShapeType {
    Circle,
    Polygon
};

/**
 * @brief Axis-Aligned Bounding Box for fast broadphase SpatialGrid queries.
 */
struct AABB {
    sf::Vector2f min;
    sf::Vector2f max;
};

/**
 * @brief Unified physics object definition replacing custom isolated entities.
 * Represents Circles, Polygons, Obstacles, and Static Walls through mass, 
 * restitution, friction coefficients, and inertia tensors. Includes triggers for specialized zones.
 */
class RigidBody {
private:
    BodyType bodyType{BodyType::Dynamic};
    ShapeType shapeType{ShapeType::Circle};

    // Kinematic State Framework
    sf::Vector2f position{0.f, 0.f};           // Center of mass coordinates
    sf::Vector2f velocity{0.f, 0.f};           // Linear velocity vector
    sf::Vector2f forceAccumulator{0.f, 0.f};   // Integrated external forces (e.g., Gravity/Wind)
    
    float orientationAngle{0.f};               // Rotational orientation tracking in radians
    float angularVelocity{0.f};                // Angular velocity (rad/s)
    float torqueAccumulator{0.f};              // Rotational force accumulation framework

    // Mass & Inertial Tensor Inversions
    float mass{1.f};                           // Absolute mass value
    float inverseMass{1.f};                    // 0.f explicitly flags Static Walls / Infinite Mass Obstacles
    float momentOfInertia{1.f};                // Mass distribution scalar factor
    float inverseInertia{1.f};                 // 0.f explicitly locks rotational stability for static bodies

    // Material Mechanical Coefficients
    float restitution{0.4f};                    // Elastic bounce coefficient (e)
    float friction{0.3f};                       // Tangent surface kinetic friction coefficient
    float dragCoefficient{0.47f};               // Aerodynamic drag coefficient (e.g. 0.47 for sphere/circle)
    sf::Vector2f surfaceVelocity{0.f, 0.f};     // Tangential velocity for conveyor surfaces

    // Bounding Footprints & Geometric Geometry Arrays
    float radius{0.f};                         // Circle radius or outer boundary scalar for Spatial Grid queries
    std::vector<sf::Vector2f> localVertices;  // Centered local mesh vertex definitions for SAT projections

    // Environmental Field System Interconnection
    bool triggerFlag{false};                   // isTrigger flag to bypass structural collision impulses
    bool gravityFreeFlag{false};               // Excludes body from global gravity acceleration
    bool collisionFreeFlag{false};             // Bypasses collision resolution pipeline

    // Lifecycle & Presentation Metadata Flags
    bool deadFlag{false};                      // Triggers deferred deletion removal passes within LevelManager
    bool visibleFlag{true};                    // Controls viewport rendering pipeline execution paths
    bool staticLevelMapFlag{false};            // Flag for static map structures baked into batch vertex arrays
    sf::Color color{sf::Color::White};         // Rendering color identifier
    std::string assetTag{""};                  // Maps data profile to localized texture assets without coupling dependencies

public:
    // Explicit constructor configuration optimized for Circular Entities
    explicit RigidBody(sf::Vector2f spawnPosition, 
                       float circleRadius, 
                       sf::Vector2f initialVelocity = {0.f, 0.f}, 
                       float bodyMass = 1.f,
                       float bodyRestitution = 0.4f,
                       float bodyFriction = 0.3f,
                       bool isTriggerZone = false) noexcept;

    // Explicit constructor configuration optimized for Convex Polygonal Entities / Extruded Lines
    explicit RigidBody(sf::Vector2f spawnPosition,
                       const std::vector<sf::Vector2f>& vertices,
                       sf::Vector2f initialVelocity = {0.f, 0.f},
                       float bodyMass = 1.f,
                       float bodyRestitution = 0.4f,
                       float bodyFriction = 0.3f,
                       bool isTriggerZone = false) noexcept;

    // Core kinematic linear/rotational update mechanics
    void updatePhysics(float globalAirDensity, float dt = 0.016667f) noexcept;
    
    // External Field Integrators & Impulse Applications
    void addForce(sf::Vector2f force) noexcept;
    void addForceAtPoint(sf::Vector2f force, sf::Vector2f worldPoint) noexcept;
    void addImpulse(sf::Vector2f impulse, sf::Vector2f contactVector) noexcept;
    void addTorque(float torque) noexcept;
    void clearForces() noexcept;

    // Inertia & Mass Recalculation
    void recalculateMassProperties() noexcept;

    // Geometry Helpers
    [[nodiscard]] std::vector<sf::Vector2f> getWorldVertices() const noexcept;
    [[nodiscard]] AABB getAABB() const noexcept;

    // Accessors & Mutators
    [[nodiscard]] BodyType getBodyType() const noexcept { return bodyType; }
    void setBodyType(BodyType type) noexcept;

    [[nodiscard]] ShapeType getType() const noexcept { return shapeType; }
    
    [[nodiscard]] sf::Vector2f getPosition() const noexcept { return position; }
    void setPosition(sf::Vector2f pos) noexcept;
    
    [[nodiscard]] sf::Vector2f getVelocity() const noexcept { return velocity; }
    void setVelocity(sf::Vector2f vel) noexcept;
    
    [[nodiscard]] float getRotationAngle() const noexcept { return orientationAngle; }
    void setRotationAngle(float radians) noexcept;
    
    [[nodiscard]] float getAngularVelocity() const noexcept { return angularVelocity; }
    void setAngularVelocity(float w) noexcept;
    
    [[nodiscard]] float getRadius() const noexcept { return radius; }
    void setRadius(float r) noexcept;

    [[nodiscard]] const std::vector<sf::Vector2f>& getLocalVertices() const noexcept { return localVertices; }
    
    [[nodiscard]] float getMass() const noexcept { return mass; }
    void setMass(float m) noexcept;

    [[nodiscard]] float getInverseMass() const noexcept { return inverseMass; }
    [[nodiscard]] float getInverseInertia() const noexcept { return inverseInertia; }
    [[nodiscard]] float getRestitution() const noexcept { return restitution; }
    void setRestitution(float e) noexcept { restitution = e; }

    [[nodiscard]] float getFriction() const noexcept { return friction; }
    void setFriction(float f) noexcept { friction = f; }

    [[nodiscard]] float getDragCoefficient() const noexcept { return dragCoefficient; }
    void setDragCoefficient(float cd) noexcept { dragCoefficient = cd; }

    [[nodiscard]] sf::Vector2f getSurfaceVelocity() const noexcept { return surfaceVelocity; }
    void setSurfaceVelocity(sf::Vector2f vel) noexcept { surfaceVelocity = vel; }

    [[nodiscard]] bool isTrigger() const noexcept { return triggerFlag; }
    void setTrigger(bool state) noexcept { triggerFlag = state; }

    [[nodiscard]] bool isGravityFree() const noexcept { return gravityFreeFlag; }
    void setGravityFree(bool state) noexcept { gravityFreeFlag = state; }

    [[nodiscard]] bool isCollisionFree() const noexcept { return collisionFreeFlag; }
    void setCollisionFree(bool state) noexcept { collisionFreeFlag = state; }

    // --- Lifecycle Interface Operations ---
    [[nodiscard]] bool isDead() const noexcept { return deadFlag; }
    void setDead(bool state) noexcept { deadFlag = state; }

    // --- Presentation Interface Operations ---
    [[nodiscard]] bool isVisible() const noexcept { return visibleFlag; }
    void setVisible(bool state) noexcept { visibleFlag = state; }

    [[nodiscard]] bool isStaticLevelMap() const noexcept { return staticLevelMapFlag; }
    void setStaticLevelMap(bool state) noexcept { staticLevelMapFlag = state; }

    [[nodiscard]] sf::Color getColor() const noexcept { return color; }
    void setColor(sf::Color c) noexcept { color = c; }

    [[nodiscard]] const std::string& getAssetTag() const noexcept { return assetTag; }
    void setAssetTag(const std::string& tag) noexcept { assetTag = tag; }
};
