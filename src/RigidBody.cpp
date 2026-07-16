#include "RigidBody.hpp"
#include "VectorMath.hpp"
#include <cmath>
#include <algorithm>

RigidBody::RigidBody(sf::Vector2f spawnPosition, 
                     float circleRadius, 
                     sf::Vector2f initialVelocity, 
                     float bodyMass,
                     float bodyRestitution,
                     float bodyFriction,
                     bool isTriggerZone) noexcept
{
    this->shapeType        = ShapeType::Circle;
    this->position         = spawnPosition;
    this->radius           = circleRadius;
    this->velocity         = initialVelocity;
    this->forceAccumulator = sf::Vector2f(0.f, 0.f);
    
    this->orientationAngle = 0.f;
    this->angularVelocity  = 0.f;
    this->torqueAccumulator = 0.f;
    
    this->restitution      = bodyRestitution;
    this->friction         = bodyFriction;
    this->triggerFlag      = isTriggerZone;
    this->mass             = bodyMass;

    // Core state setup for lifecycle and visibility
    this->deadFlag         = false;
    this->visibleFlag      = true;
    this->assetTag         = "";

    if (this->mass > 0.f) {
        this->inverseMass = 1.f / this->mass;
        this->momentOfInertia = 0.5f * this->mass * (this->radius * this->radius);
        this->inverseInertia = 1.f / this->momentOfInertia;
    } else {
        this->inverseMass     = 0.f;
        this->momentOfInertia = 0.f;
        this->inverseInertia  = 0.f;
    }
}

RigidBody::RigidBody(sf::Vector2f spawnPosition,
                     const std::vector<sf::Vector2f>& vertices,
                     sf::Vector2f initialVelocity,
                     float bodyMass,
                     float bodyRestitution,
                     float bodyFriction,
                     bool isTriggerZone) noexcept
{
    this->shapeType        = ShapeType::Polygon;
    this->velocity         = initialVelocity;
    this->forceAccumulator = sf::Vector2f(0.f, 0.f);
    
    this->orientationAngle = 0.f;
    this->angularVelocity  = 0.f;
    this->torqueAccumulator = 0.f;
    
    this->restitution      = bodyRestitution;
    this->friction         = bodyFriction;
    this->triggerFlag      = isTriggerZone;
    this->radius           = 0.f;

    // Core state setup for lifecycle and visibility
    this->deadFlag         = false;
    this->visibleFlag      = true;
    this->assetTag         = "";

    // 1. Compute the mathematical arithmetic centroid (Center of Mass)
    sf::Vector2f centerOfMass(0.f, 0.f);
    for (const auto& vertex : vertices) {
        centerOfMass += vertex;
    }
    if (!vertices.empty()) {
        centerOfMass /= static_cast<float>(vertices.size());
    }

    // 2. Map and localize vertices relative to the calculated origin center
    this->localVertices.reserve(vertices.size());
    float maxRadiusSq = 0.f;
    for (const auto& vertex : vertices) {
        sf::Vector2f centeredVertex = vertex - centerOfMass;
        this->localVertices.push_back(centeredVertex);

        float distSq = Math::lengthSq(centeredVertex);
        if (distSq > maxRadiusSq) {
            maxRadiusSq = distSq;
        }
    }
    
    this->radius   = std::sqrt(maxRadiusSq);
    this->position = spawnPosition + centerOfMass; 
    this->mass     = bodyMass;

    // 3. Evaluate polygonal angular inertia profiles
    if (this->mass > 0.f) {
        this->inverseMass = 1.f / this->mass;
        this->momentOfInertia = 0.4f * this->mass * maxRadiusSq;
        this->inverseInertia = 1.f / this->momentOfInertia;
    } else {
        this->inverseMass     = 0.f;
        this->momentOfInertia = 0.f;
        this->inverseInertia  = 0.f;
    }
}

void RigidBody::updatePhysics(float airDragCoefficient) noexcept {
    if (inverseMass == 0.f) return;

    velocity        *= (1.f - airDragCoefficient);
    angularVelocity *= (1.f - airDragCoefficient);
}

void RigidBody::addForce(sf::Vector2f force) noexcept {
    if (inverseMass == 0.f) return;
    forceAccumulator += force;
}

void RigidBody::addTorque(float torque) noexcept {
    if (inverseInertia == 0.f) return;
    torqueAccumulator += torque;
}

void RigidBody::clearForces() noexcept {
    forceAccumulator  = sf::Vector2f(0.f, 0.f);
    torqueAccumulator = 0.f;
}

void RigidBody::setPosition(sf::Vector2f pos) noexcept {
    position = pos;
}

void RigidBody::setRotationAngle(float radians) noexcept {
    orientationAngle = radians;
}