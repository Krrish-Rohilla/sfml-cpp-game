#include "PhysicsSystem.hpp"
#include "VectorMath.hpp"
#include "Spring.hpp"
#include "SpatialGrid.hpp"
#include "RigidBody.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace {
    [[nodiscard]] std::vector<sf::Vector2f> getTransformWorldVertices(const RigidBody* body) noexcept {
        std::vector<sf::Vector2f> worldVertices;
        const auto& localVertices = body->getLocalVertices();
        worldVertices.reserve(localVertices.size());

        const float cosTheta = std::cos(body->getRotationAngle());
        const float sinTheta = std::sin(body->getRotationAngle());
        const sf::Vector2f position = body->getPosition();

        for (const auto& localVert : localVertices) {
            sf::Vector2f rotatedVert{
                localVert.x * cosTheta - localVert.y * sinTheta,
                localVert.x * sinTheta + localVert.y * cosTheta
            };
            worldVertices.push_back(rotatedVert + position);
        }
        return worldVertices;
    }

    void projectPolygon(const std::vector<sf::Vector2f>& vertices, const sf::Vector2f& axis, float& minProj, float& maxProj) noexcept {
        minProj = Math::dot(vertices[0], axis);
        maxProj = minProj;
        for (size_t i = 1; i < vertices.size(); ++i) {
            float projection = Math::dot(vertices[i], axis);
            if (projection < minProj) minProj = projection;
            if (projection > maxProj) maxProj = projection;
        }
    }
}

void PhysicsSystem::step(std::vector<std::unique_ptr<RigidBody>>& bodies,
                         std::vector<std::unique_ptr<Spring>>& springs,
                         SpatialGrid& grid,
                         float dt,
                         float gravity,
                         float airDrag) noexcept 
{
    if (dt <= 0.f) return;

    // Pass 1: Integration Pass - Process linear and rotational motion frameworks
    integrateForces(bodies, dt, gravity, airDrag);

    // Pass 2: Constraint Pass - Evaluate Hooke's Law spring lattice vectors
    evaluateConstraints(springs, dt);

    // Pass 3: Broadphase Filter - Map spatial quadrants to filter out far objects
    std::vector<std::pair<RigidBody*, RigidBody*>> potentialPairs;
    performBroadphase(bodies, grid, potentialPairs);

    // Pass 4: Narrowphase SAT - Calculate overlapping shapes and exact normals
    std::vector<ContactManifold> contactPool;
    performNarrowphase(potentialPairs, contactPool);

    // Post-Pass: Resolves physical impulses, velocity updates, and pushes shapes out of clipping
    resolveCollisions(contactPool, dt);
}

void PhysicsSystem::integrateForces(std::vector<std::unique_ptr<RigidBody>>& bodies, float dt, float gravity, float airDrag) noexcept {
    for (auto& body : bodies) {
        if (!body || body->getInverseMass() == 0.f) continue; 

        // Apply linear/rotational drag coefficients
        body->updatePhysics(airDrag);

        // Calculate combined linear acceleration: constant gravity field plus
        // whatever external forces (pushes, wind zones, impulses) were queued
        // this frame via addForce(). F = ma, so accel = force * inverseMass.
        sf::Vector2f gravityAccel(0.f, gravity);
        sf::Vector2f appliedAccel = body->getAccumulatedForce() * body->getInverseMass();
        body->setVelocity(body->getVelocity() + (gravityAccel + appliedAccel) * dt);

        // Integrate velocity to update absolute world coordinates
        body->setPosition(body->getPosition() + body->getVelocity() * dt);

        // Angular acceleration from queued torque (addTorque()): alpha = torque * inverseInertia.
        float appliedAngularAccel = body->getAccumulatedTorque() * body->getInverseInertia();
        body->setAngularVelocity(body->getAngularVelocity() + appliedAngularAccel * dt);

        // Integrate angular velocity to update orientation angle
        body->setRotationAngle(body->getRotationAngle() + body->getAngularVelocity() * dt);

        // Flush accumulators to prepare registers for the upcoming frame cycle
        body->clearForces();
    }
}

void PhysicsSystem::evaluateConstraints(std::vector<std::unique_ptr<Spring>>& springs, float dt) noexcept {
    for (auto& spring : springs) {
        if (!spring) continue;
        RigidBody* ba = spring->bodyA;
        RigidBody* bb = spring->bodyB;
        if (!ba || !bb) continue;

        sf::Vector2f deltaPos = bb->getPosition() - ba->getPosition();
        float currentLength = Math::length(deltaPos);
        if (currentLength < 0.0001f) continue;

        sf::Vector2f normalAxis = deltaPos / currentLength;
        float displacement = currentLength - spring->restLength;

        sf::Vector2f relativeVel = bb->getVelocity() - ba->getVelocity();
        float velocityAlongNormal = Math::dot(relativeVel, normalAxis);

        // Hooke's Law compliance equations processing spring and damping bounds
        float springForceMagnitude = (displacement * spring->stiffness) + (velocityAlongNormal * spring->damping);
        sf::Vector2f totalImpulse = normalAxis * (springForceMagnitude * dt);

        if (ba->getInverseMass() > 0.f) {
            ba->setVelocity(ba->getVelocity() + totalImpulse * ba->getInverseMass());
        }
        if (bb->getInverseMass() > 0.f) {
            bb->setVelocity(bb->getVelocity() - totalImpulse * bb->getInverseMass());
        }
    }
}

void PhysicsSystem::performBroadphase(std::vector<std::unique_ptr<RigidBody>>& bodies, 
                                      SpatialGrid& grid, 
                                      std::vector<std::pair<RigidBody*, RigidBody*>>& potentialPairs) noexcept 
{
    grid.clear();
    for (auto& body : bodies) {
        if (body) {
            grid.insert(body.get());
        }
    }
    grid.queryPotentialPairs(potentialPairs);
}

void PhysicsSystem::performNarrowphase(std::vector<std::pair<RigidBody*, RigidBody*>>& potentialPairs, 
                                       std::vector<ContactManifold>& contactPool) noexcept 
{
    for (auto& pair : potentialPairs) {
        RigidBody* a = pair.first;
        RigidBody* b = pair.second;

        if (!a || !b) continue;
        if (a->getInverseMass() == 0.f && b->getInverseMass() == 0.f) continue;

        ContactManifold manifold{ a, b, sf::Vector2f(0.f, 0.f), 0.f };
        bool isColliding = false;

        if (a->getType() == ShapeType::Circle && b->getType() == ShapeType::Circle) {
            isColliding = testCircleVsCircle(a, b, manifold);
        } else if (a->getType() == ShapeType::Polygon && b->getType() == ShapeType::Polygon) {
            isColliding = testPolygonVsPolygon(a, b, manifold);
        } else if (a->getType() == ShapeType::Circle && b->getType() == ShapeType::Polygon) {
            isColliding = testCircleVsPolygon(a, b, manifold);
        } else if (a->getType() == ShapeType::Polygon && b->getType() == ShapeType::Circle) {
            isColliding = testCircleVsPolygon(b, a, manifold);
            manifold.normal = -manifold.normal;
        }

        if (isColliding) {
            contactPool.push_back(manifold);
        }
    }
}

void PhysicsSystem::resolveCollisions(std::vector<ContactManifold>& contactPool, float dt) noexcept {
    for (auto& contact : contactPool) {
        RigidBody* A = contact.bodyA;
        RigidBody* B = contact.bodyB;
        if (!A || !B) continue;

        // --- Environmental Trigger Field System Bypasses ---
        if (A->isTrigger() || B->isTrigger()) {
            if (A->isTrigger() && B->getInverseMass() > 0.f) {
                B->setVelocity(B->getVelocity() + A->getVelocity() * dt);
            }
            if (B->isTrigger() && A->getInverseMass() > 0.f) {
                A->setVelocity(A->getVelocity() + B->getVelocity() * dt);
            }
            continue; 
        }

        // --- Solid Body Resolution Physics ---
        const float inverseMassSum = A->getInverseMass() + B->getInverseMass();
        if (inverseMassSum == 0.f) continue;

        // 1. Positional Correction Pass (Anti-Sinking Penetration Guard)
        const float positionalPercent = 0.4f; 
        const float positionalSlop = 0.01f;   
        sf::Vector2f nonClippingCorrection = contact.normal * 
            (std::max(contact.penetrationDepth - positionalSlop, 0.f) / inverseMassSum) * positionalPercent;
        
        A->setPosition(A->getPosition() - nonClippingCorrection * A->getInverseMass());
        B->setPosition(B->getPosition() + nonClippingCorrection * B->getInverseMass());

        // 2. High-Grade Matrix-Free Linear & Angular Impulse Resolutions
        sf::Vector2f pointContactVectorA = contact.normal * (A->getRadius() - contact.penetrationDepth * 0.5f);
        sf::Vector2f pointContactVectorB = -contact.normal * (B->getRadius() - contact.penetrationDepth * 0.5f);

        sf::Vector2f contactVelocityA = A->getVelocity() + sf::Vector2f(-A->getAngularVelocity() * pointContactVectorA.y, A->getAngularVelocity() * pointContactVectorA.x);
        sf::Vector2f contactVelocityB = B->getVelocity() + sf::Vector2f(-B->getAngularVelocity() * pointContactVectorB.y, B->getAngularVelocity() * pointContactVectorB.x);
        sf::Vector2f relativeVelocity = contactVelocityB - contactVelocityA;

        float relativeNormalVelocity = Math::dot(relativeVelocity, contact.normal);
        if (relativeNormalVelocity > 0.f) continue; 

        float restitutionCoefficient = std::min(A->getRestitution(), B->getRestitution());

        float crossA = Math::cross(pointContactVectorA, contact.normal);
        float crossB = Math::cross(pointContactVectorB, contact.normal);
        float angularMassTerm = (crossA * crossA * A->getInverseInertia()) + (crossB * crossB * B->getInverseInertia());

        float impulseScalar = -(1.f + restitutionCoefficient) * relativeNormalVelocity / (inverseMassSum + angularMassTerm);

        sf::Vector2f normalImpulseVector = contact.normal * impulseScalar;
        A->setVelocity(A->getVelocity() - normalImpulseVector * A->getInverseMass());
        B->setVelocity(B->getVelocity() + normalImpulseVector * B->getInverseMass());
        A->setAngularVelocity(A->getAngularVelocity() - crossA * impulseScalar * A->getInverseInertia());
        B->setAngularVelocity(B->getAngularVelocity() + crossB * impulseScalar * B->getInverseInertia());

        // 3. Friction Impulse Resolution
        contactVelocityA = A->getVelocity() + sf::Vector2f(-A->getAngularVelocity() * pointContactVectorA.y, A->getAngularVelocity() * pointContactVectorA.x);
        contactVelocityB = B->getVelocity() + sf::Vector2f(-B->getAngularVelocity() * pointContactVectorB.y, B->getAngularVelocity() * pointContactVectorB.x);
        relativeVelocity = contactVelocityB - contactVelocityA;

        sf::Vector2f surfaceTangentAxis = relativeVelocity - contact.normal * Math::dot(relativeVelocity, contact.normal);
        float tangentMagnitude = Math::length(surfaceTangentAxis);

        if (tangentMagnitude > 0.0001f) {
            surfaceTangentAxis /= tangentMagnitude;

            float crossTangentA = Math::cross(pointContactVectorA, surfaceTangentAxis);
            float crossTangentB = Math::cross(pointContactVectorB, surfaceTangentAxis);
            float frictionAngularTerm = (crossTangentA * crossTangentA * A->getInverseInertia()) + (crossTangentB * crossTangentB * B->getInverseInertia());

            float frictionImpulseScalar = -Math::dot(relativeVelocity, surfaceTangentAxis) / (inverseMassSum + frictionAngularTerm);

            float staticFrictionBlend = std::sqrt(A->getFriction() * B->getFriction());
            float maximumFrictionLimit = impulseScalar * staticFrictionBlend;
            frictionImpulseScalar = std::clamp(frictionImpulseScalar, -maximumFrictionLimit, maximumFrictionLimit);

            sf::Vector2f frictionImpulseVector = surfaceTangentAxis * frictionImpulseScalar;
            A->setVelocity(A->getVelocity() - frictionImpulseVector * A->getInverseMass());
            B->setVelocity(B->getVelocity() + frictionImpulseVector * B->getInverseMass());
            A->setAngularVelocity(A->getAngularVelocity() - crossTangentA * frictionImpulseScalar * A->getInverseInertia());
            B->setAngularVelocity(B->getAngularVelocity() + crossTangentB * frictionImpulseScalar * B->getInverseInertia());
        }
    }
}

bool PhysicsSystem::testCircleVsCircle(RigidBody* a, RigidBody* b, ContactManifold& manifold) noexcept {
    sf::Vector2f vectorDelta = b->getPosition() - a->getPosition();
    float totalDistanceSquared = Math::lengthSq(vectorDelta);
    float targetedRadiusSum = a->getRadius() + b->getRadius();

    if (totalDistanceSquared >= targetedRadiusSum * targetedRadiusSum) return false;

    float totalDistance = std::sqrt(totalDistanceSquared);
    manifold.normal = (totalDistance != 0.f) ? vectorDelta / totalDistance : sf::Vector2f(0.f, 1.f);
    manifold.penetrationDepth = targetedRadiusSum - totalDistance;
    return true;
}

bool PhysicsSystem::testPolygonVsPolygon(RigidBody* a, RigidBody* b, ContactManifold& manifold) noexcept {
    std::vector<sf::Vector2f> worldVertsA = getTransformWorldVertices(a);
    std::vector<sf::Vector2f> worldVertsB = getTransformWorldVertices(b);

    float minimumPenetration = std::numeric_limits<float>::max();
    sf::Vector2f selectedCollisionAxis(0.f, 0.f);

    auto testAxesGroup = [&](const std::vector<sf::Vector2f>& vertices) noexcept -> bool {
        for (size_t i = 0; i < vertices.size(); ++i) {
            sf::Vector2f currentEdge = vertices[(i + 1) % vertices.size()] - vertices[i];
            sf::Vector2f normalAxis = Math::normalize(Math::Normal(currentEdge));

            float minA, maxA, minB, maxB;
            projectPolygon(worldVertsA, normalAxis, minA, maxA);
            projectPolygon(worldVertsB, normalAxis, minB, maxB);

            if (maxA < minB || maxB < minA) return false; 

            float currentOverlap = std::min(maxA, maxB) - std::max(minA, minB);
            if (currentOverlap < minimumPenetration) {
                minimumPenetration = currentOverlap;
                selectedCollisionAxis = normalAxis;
            }
        }
        return true;
    };

    if (!testAxesGroup(worldVertsA) || !testAxesGroup(worldVertsB)) return false;

    if (Math::dot(selectedCollisionAxis, b->getPosition() - a->getPosition()) < 0.f) {
        selectedCollisionAxis = -selectedCollisionAxis;
    }

    manifold.normal = selectedCollisionAxis;
    manifold.penetrationDepth = minimumPenetration;
    return true;
}

bool PhysicsSystem::testCircleVsPolygon(RigidBody* circle, RigidBody* polygon, ContactManifold& manifold) noexcept {
    std::vector<sf::Vector2f> polygonWorldVerts = getTransformWorldVertices(polygon);
    sf::Vector2f centerPosition = circle->getPosition();

    float minimumPenetration = std::numeric_limits<float>::max();
    sf::Vector2f selectedCollisionAxis(0.f, 0.f);

    for (size_t i = 0; i < polygonWorldVerts.size(); ++i) {
        sf::Vector2f currentEdge = polygonWorldVerts[(i + 1) % polygonWorldVerts.size()] - polygonWorldVerts[i];
        sf::Vector2f normalAxis = Math::normalize(Math::Normal(currentEdge));

        float minP, maxP;
        projectPolygon(polygonWorldVerts, normalAxis, minP, maxP);

        float circleCenterProj = Math::dot(centerPosition, normalAxis);
        float minC = circleCenterProj - circle->getRadius();
        float maxC = circleCenterProj + circle->getRadius();

        if (maxP < minC || maxC < minP) return false;

        float currentOverlap = std::min(maxP, maxC) - std::max(minP, minC);
        if (currentOverlap < minimumPenetration) {
            minimumPenetration = currentOverlap;
            selectedCollisionAxis = normalAxis;
        }
    }

    size_t closestVertexIndex = 0;
    float closestDistanceSquared = std::numeric_limits<float>::max();
    for (size_t i = 0; i < polygonWorldVerts.size(); ++i) {
        float distSq = Math::lengthSq(centerPosition - polygonWorldVerts[i]);
        if (distSq < closestDistanceSquared) {
            closestDistanceSquared = distSq;
            closestVertexIndex = i;
        }
    }

    sf::Vector2f circleToVertexAxis = centerPosition - polygonWorldVerts[closestVertexIndex];
    if (Math::lengthSq(circleToVertexAxis) > 0.0001f) {
        circleToVertexAxis = Math::normalize(circleToVertexAxis);

        float minP, maxP;
        projectPolygon(polygonWorldVerts, circleToVertexAxis, minP, maxP);

        float circleCenterProj = Math::dot(centerPosition, circleToVertexAxis);
        float minC = circleCenterProj - circle->getRadius();
        float maxC = circleCenterProj + circle->getRadius();

        if (maxP < minC || maxC < minP) return false;

        float currentOverlap = std::min(maxP, maxC) - std::max(minP, minC);
        if (currentOverlap < minimumPenetration) {
            minimumPenetration = currentOverlap;
            selectedCollisionAxis = circleToVertexAxis;
        }
    }

    if (Math::dot(selectedCollisionAxis, polygon->getPosition() - centerPosition) < 0.f) {
        selectedCollisionAxis = -selectedCollisionAxis;
    }

    manifold.bodyA = circle;
    manifold.bodyB = polygon;
    manifold.normal = selectedCollisionAxis;
    manifold.penetrationDepth = minimumPenetration;
    return true;
}