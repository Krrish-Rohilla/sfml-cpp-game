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

    this->deadFlag         = false;
    this->visibleFlag      = true;
    this->color            = sf::Color::White;
    this->assetTag         = "";

    recalculateMassProperties();
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

    this->deadFlag         = false;
    this->visibleFlag      = true;
    this->color            = sf::Color::White;
    this->assetTag         = "";

    // 1. Compute true area-weighted centroid (Center of Mass) via Green's Theorem / Shoelace formula
    sf::Vector2f centerOfMass(0.f, 0.f);
    float areaDouble = 0.f;

    if (!vertices.empty()) {
        sf::Vector2f centroidSum(0.f, 0.f);
        size_t numVerts = vertices.size();

        for (size_t i = 0; i < numVerts; ++i) {
            sf::Vector2f p1 = vertices[i];
            sf::Vector2f p2 = vertices[(i + 1) % numVerts];

            float k = p1.x * p2.y - p2.x * p1.y;
            areaDouble += k;
            centroidSum += (p1 + p2) * k;
        }

        if (std::abs(areaDouble) > 0.00001f) {
            centerOfMass = centroidSum / (3.f * areaDouble);
        } else {
            // Fallback to arithmetic mean for degenerate/collinear points
            for (const auto& vertex : vertices) {
                centerOfMass += vertex;
            }
            centerOfMass /= static_cast<float>(vertices.size());
        }
    }

    // 2. Localize vertices relative to origin center and enforce CCW winding order
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

    // Ensure Counter-Clockwise (CCW) winding order for SAT collision detection
    if (areaDouble < 0.f) {
        std::reverse(this->localVertices.begin(), this->localVertices.end());
    }
    
    this->radius   = std::sqrt(maxRadiusSq);
    this->position = spawnPosition + centerOfMass; 
    this->mass     = bodyMass;

    recalculateMassProperties();
}

void RigidBody::recalculateMassProperties() noexcept {
    if (bodyType == BodyType::Static || bodyType == BodyType::Kinematic || mass <= 0.f) {
        inverseMass     = 0.f;
        momentOfInertia = 0.f;
        inverseInertia  = 0.f;
        return;
    }

    inverseMass = 1.f / mass;

    if (shapeType == ShapeType::Circle) {
        momentOfInertia = 0.5f * mass * (radius * radius);
    } else { // Polygon: Calculate exact second moment of area tensor about centroid
        if (!localVertices.empty()) {
            float areaDouble = 0.f;
            float iSum = 0.f;
            size_t numVerts = localVertices.size();

            for (size_t i = 0; i < numVerts; ++i) {
                sf::Vector2f p1 = localVertices[i];
                sf::Vector2f p2 = localVertices[(i + 1) % numVerts];

                float k = p1.x * p2.y - p2.x * p1.y;
                areaDouble += k;
                iSum += k * (p1.x * p1.x + p1.x * p2.x + p2.x * p2.x + 
                             p1.y * p1.y + p1.y * p2.y + p2.y * p2.y);
            }

            if (std::abs(areaDouble) > 0.00001f) {
                momentOfInertia = std::abs(mass * (iSum / (6.f * areaDouble)));
            } else {
                momentOfInertia = 0.4f * mass * (radius * radius);
            }
        } else {
            momentOfInertia = 0.4f * mass * (radius * radius);
        }
    }

    if (momentOfInertia > 0.f) {
        inverseInertia = 1.f / momentOfInertia;
    } else {
        inverseInertia = 0.f;
    }
}

void RigidBody::setBodyType(BodyType type) noexcept {
    bodyType = type;
    recalculateMassProperties();
}

void RigidBody::setMass(float m) noexcept {
    mass = m;
    recalculateMassProperties();
}

void RigidBody::setRadius(float r) noexcept {
    radius = r;
    recalculateMassProperties();
}

void RigidBody::updatePhysics(float globalAirDensity, float dt) noexcept {
    if (bodyType == BodyType::Static) return;

    // 1. External Forces, Torques, & Aerodynamic Drag Integration (Dynamic bodies only)
    if (bodyType == BodyType::Dynamic && inverseMass > 0.f) {
        // Integrate Accumulated Linear External Forces & Torques
        velocity += (forceAccumulator * inverseMass) * dt;
        if (inverseInertia > 0.f) {
            angularVelocity += (torqueAccumulator * inverseInertia) * dt;
        }

        // Quadratic Aerodynamic Drag: F_drag = -0.5 * rho * Cd * Area * |v| * v
        // Frontal cross-sectional dimension in 2D space (proportional to 2 * radius)
        float frontalArea = (radius > 0.001f) ? (2.f * radius) : 1.f;

        float speed = Math::length(velocity);
        if (speed > 0.0001f) {
            float dragForceMag = 0.5f * globalAirDensity * dragCoefficient * frontalArea * (speed * speed);
            sf::Vector2f dragForce = -(velocity / speed) * dragForceMag;

            // Acceleration = Force / Mass (Heavier mass => smaller deceleration)
            sf::Vector2f dragAccel = dragForce * inverseMass;
            sf::Vector2f velDelta = dragAccel * dt;

            if (Math::lengthSq(velDelta) >= speed * speed) {
                velocity = sf::Vector2f(0.f, 0.f);
            } else {
                velocity += velDelta;
            }
        }

        // Rotational Aerodynamic Resistance / Angular Damping
        float absOmega = std::abs(angularVelocity);
        if (absOmega > 0.0001f && inverseInertia > 0.f) {
            float rotDragTorque = 0.5f * globalAirDensity * dragCoefficient * (radius * radius * radius) * (absOmega * absOmega);
            float alphaDrag = rotDragTorque * inverseInertia;
            float omegaDelta = alphaDrag * dt;

            if (omegaDelta >= absOmega) {
                angularVelocity = 0.f;
            } else {
                angularVelocity -= (angularVelocity > 0.f ? 1.f : -1.f) * omegaDelta;
            }
        }

        // Clear accumulated forces after integration
        clearForces();
    }

    // 2. Position and Orientation Integration (Runs for both Dynamic AND Kinematic bodies)
    position += velocity * dt;
    orientationAngle += angularVelocity * dt;
}

void RigidBody::addForce(sf::Vector2f force) noexcept {
    if (bodyType == BodyType::Static || bodyType == BodyType::Kinematic || inverseMass == 0.f) return;
    forceAccumulator += force;
}

void RigidBody::addForceAtPoint(sf::Vector2f force, sf::Vector2f worldPoint) noexcept {
    if (bodyType == BodyType::Static || bodyType == BodyType::Kinematic || inverseMass == 0.f) return;
    forceAccumulator += force;

    // Torque = cross(r, F)
    sf::Vector2f r = worldPoint - position;
    float torque = Math::cross(r, force);
    torqueAccumulator += torque;
}

void RigidBody::addImpulse(sf::Vector2f impulse, sf::Vector2f contactVector) noexcept {
    if (bodyType == BodyType::Static || bodyType == BodyType::Kinematic || inverseMass == 0.f) return;

    velocity += impulse * inverseMass;
    if (inverseInertia > 0.f) {
        angularVelocity += Math::cross(contactVector, impulse) * inverseInertia;
    }
}

void RigidBody::addTorque(float torque) noexcept {
    if (bodyType == BodyType::Static || bodyType == BodyType::Kinematic || inverseInertia == 0.f) return;
    torqueAccumulator += torque;
}

void RigidBody::clearForces() noexcept {
    forceAccumulator  = sf::Vector2f(0.f, 0.f);
    torqueAccumulator = 0.f;
}

void RigidBody::setPosition(sf::Vector2f pos) noexcept {
    position = pos;
}

void RigidBody::setVelocity(sf::Vector2f vel) noexcept {
    if (bodyType == BodyType::Static) return;
    velocity = vel;
}

void RigidBody::setRotationAngle(float radians) noexcept {
    orientationAngle = radians;
}

void RigidBody::setAngularVelocity(float w) noexcept {
    if (bodyType == BodyType::Static) return;
    angularVelocity = w;
}

std::vector<sf::Vector2f> RigidBody::getWorldVertices() const noexcept {
    std::vector<sf::Vector2f> worldVerts;
    if (shapeType == ShapeType::Circle) return worldVerts;

    worldVerts.reserve(localVertices.size());
    const float cosA = std::cos(orientationAngle);
    const float sinA = std::sin(orientationAngle);

    for (const auto& localV : localVertices) {
        // Rotate local vertex by orientation angle and translate to world position
        sf::Vector2f rotatedV(
            localV.x * cosA - localV.y * sinA,
            localV.x * sinA + localV.y * cosA
        );
        worldVerts.push_back(position + rotatedV);
    }
    return worldVerts;
}

AABB RigidBody::getAABB() const noexcept {
    AABB bounds;
    if (shapeType == ShapeType::Circle) {
        bounds.min = position - sf::Vector2f(radius, radius);
        bounds.max = position + sf::Vector2f(radius, radius);
    } else {
        auto worldVerts = getWorldVertices();
        if (worldVerts.empty()) {
            bounds.min = position;
            bounds.max = position;
        } else {
            bounds.min = worldVerts[0];
            bounds.max = worldVerts[0];
            for (size_t i = 1; i < worldVerts.size(); ++i) {
                bounds.min.x = std::min(bounds.min.x, worldVerts[i].x);
                bounds.min.y = std::min(bounds.min.y, worldVerts[i].y);
                bounds.max.x = std::max(bounds.max.x, worldVerts[i].x);
                bounds.max.y = std::max(bounds.max.y, worldVerts[i].y);
            }
        }
    }
    return bounds;
}
