#include "SoftBody.hpp"
#include "RigidBody.hpp"
#include "LevelManager.hpp"
#include "VectorMath.hpp"
#include <cmath>

SoftBody SoftBody::createBlob(LevelManager& manager, 
                               sf::Vector2f center, 
                               float radius, 
                               float particleRadius, 
                               std::size_t pointCount, 
                               float totalMass, 
                               float stiffness, 
                               float damping) noexcept 
{
    SoftBody body;
    if (pointCount < 3) return body;

    body.particles.reserve(pointCount + 1);

    // 1. Create a central structural anchor to distribute internal volume pressure
    const float centerMass = totalMass * 0.15f;
    RigidBody* centerNode = manager.spawnCircle(center, particleRadius, {0.f, 0.f}, centerMass, 0.1f, 0.5f, false);
    if (centerNode) {
        body.particles.push_back(centerNode);
    }

    // 2. Spawn outer perimeter particles in a uniform circular path
    const float perimeterMass = (totalMass * 0.85f) / static_cast<float>(pointCount);
    const float angleStep = 2.f * 3.14159265f / static_cast<float>(pointCount);

    for (std::size_t i = 0; i < pointCount; ++i) {
        const float angle = static_cast<float>(i) * angleStep;
        const sf::Vector2f offset{radius * std::cos(angle), radius * std::sin(angle)};
        
        RigidBody* perimeterNode = manager.spawnCircle(center + offset, particleRadius, {0.f, 0.f}, perimeterMass, 0.1f, 0.5f, false);
        if (perimeterNode) {
            body.particles.push_back(perimeterNode);
        }
    }

    // 3. Connect structural mass-spring links (skipping index 0 which is the center node)
    const std::size_t nodeCount = body.particles.size();
    for (std::size_t i = 1; i < nodeCount; ++i) {
        RigidBody* current = body.particles[i];
        
        // Connect structural link to the center node to prevent collapse
        if (centerNode) {
            manager.addSpring(current, centerNode, radius, stiffness, damping);
        }

        // Connect outer perimeter link to adjacent neighbor
        std::size_t nextIndex = i + 1;
        if (nextIndex >= nodeCount) {
            nextIndex = 1; // Wrap around to the first perimeter node
        }
        RigidBody* next = body.particles[nextIndex];

        const float restLength = Math::length(next->getPosition() - current->getPosition());
        manager.addSpring(current, next, restLength, stiffness, damping);
    }

    return body;
}

SoftBody SoftBody::createRope(LevelManager& manager, 
                               sf::Vector2f start, 
                               sf::Vector2f end, 
                               std::size_t segmentCount, 
                               float particleRadius, 
                               float totalMass, 
                               float stiffness, 
                               float damping) noexcept 
{
    SoftBody body;
    if (segmentCount == 0) return body;

    const std::size_t nodeCount = segmentCount + 1;
    body.particles.reserve(nodeCount);

    const sf::Vector2f totalDelta = end - start;
    const sf::Vector2f segmentStep = totalDelta / static_cast<float>(segmentCount);
    const float nodeMass = totalMass / static_cast<float>(nodeCount);
    const float restLength = Math::length(segmentStep);

    // 1. Instantiate the linear sequence of point-mass elements
    for (std::size_t i = 0; i < nodeCount; ++i) {
        const sf::Vector2f position = start + (segmentStep * static_cast<float>(i));
        
        // Note: The first and last nodes can be toggled as static (mass = 0) externally if desired
        RigidBody* node = manager.spawnCircle(position, particleRadius, {0.f, 0.f}, nodeMass, 0.05f, 0.8f, false);
        if (node) {
            body.particles.push_back(node);
        }
    }

    // 2. Construct sequential spring connections across adjacent indices
    for (std::size_t i = 0; i < segmentCount; ++i) {
        if (i + 1 < body.particles.size()) {
            manager.addSpring(body.particles[i], body.particles[i + 1], restLength, stiffness, damping);
        }
    }

    return body;
}

SoftBody SoftBody::createGrid(LevelManager& manager, 
                               sf::Vector2f topLeft, 
                               float width, 
                               float height, 
                               std::size_t columns, 
                               std::size_t rows, 
                               float particleRadius, 
                               float totalMass, 
                               float stiffness, 
                               float damping) noexcept 
{
    SoftBody body;
    if (columns < 2 || rows < 2) return body;

    const std::size_t totalNodes = columns * rows;
    body.particles.resize(totalNodes, nullptr);

    const float dx = width / static_cast<float>(columns - 1);
    const float dy = height / static_cast<float>(rows - 1);
    const float nodeMass = totalMass / static_cast<float>(totalNodes);

    // 1. Populate the 2D spatial node grid
    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < columns; ++c) {
            const sf::Vector2f position{
                topLeft.x + static_cast<float>(c) * dx,
                topLeft.y + static_cast<float>(r) * dy
            };
            
            RigidBody* node = manager.spawnCircle(position, particleRadius, {0.f, 0.f}, nodeMass, 0.1f, 0.6f, false);
            body.particles[r * columns + c] = node;
        }
    }

    // Helper lambda to safely obtain nodes at index coordinates
    auto getNode = [&](std::size_t c, std::size_t r) noexcept -> RigidBody* {
        if (c >= columns || r >= rows) return nullptr;
        return body.particles[r * columns + c];
    };

    // 2. Generate Structural, Shear, and Bending mass-spring networks
    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < columns; ++c) {
            RigidBody* current = getNode(c, r);
            if (!current) continue;

            // --- Structural Springs (Horizontal & Vertical Neighbors) ---
            if (RigidBody* right = getNode(c + 1, r)) {
                manager.addSpring(current, right, dx, stiffness, damping);
            }
            if (RigidBody* down = getNode(c, r + 1)) {
                manager.addSpring(current, down, dy, stiffness, damping);
            }

            // --- Shear Springs (Diagonal Neighbors for Torsional Stability) ---
            const float diagonalLength = std::sqrt(dx * dx + dy * dy);
            if (RigidBody* diagonalRight = getNode(c + 1, r + 1)) {
                manager.addSpring(current, diagonalRight, diagonalLength, stiffness, damping);
            }
            if (RigidBody* diagonalLeft = getNode(c - 1, r + 1)) {
                manager.addSpring(current, diagonalLeft, diagonalLength, stiffness, damping);
            }

            // --- Bending Springs (Structural Resistance against Creasing / Folding) ---
            if (RigidBody* right2 = getNode(c + 2, r)) {
                manager.addSpring(current, right2, dx * 2.f, stiffness, damping);
            }
            if (RigidBody* down2 = getNode(c, r + 2)) {
                manager.addSpring(current, down2, dy * 2.f, stiffness, damping);
            }
        }
    }

    // Filter out potential nullptr values from spawning failures
    body.particles.erase(
        std::remove(body.particles.begin(), body.particles.end(), nullptr),
        body.particles.end()
    );

    return body;
}

void SoftBody::applyForce(sf::Vector2f force) noexcept {
    for (auto* particle : particles) {
        if (particle) {
            particle->addForce(force);
        }
    }
}

void SoftBody::setVelocity(sf::Vector2f velocity) noexcept {
    for (auto* particle : particles) {
        if (particle) {
            particle->setVelocity(velocity);
        }
    }
}

void SoftBody::setVisible(bool visible) noexcept {
    for (auto* particle : particles) {
        if (particle) {
            particle->setVisible(visible);
        }
    }
}

void SoftBody::destroy() noexcept {
    for (auto* particle : particles) {
        if (particle) {
            particle->setDead(true);
        }
    }
    particles.clear();
    constraints.clear();
}

sf::Vector2f SoftBody::getCenterOfMass() const noexcept {
    if (particles.empty()) return {0.f, 0.f};

    sf::Vector2f totalPos{0.f, 0.f};
    float totalMass = 0.f;

    for (const auto* particle : particles) {
        if (particle && particle->getInverseMass() > 0.f) {
            const float m = particle->getMass();
            totalPos += particle->getPosition() * m;
            totalMass += m;
        }
    }

    if (totalMass > 0.f) {
        return totalPos / totalMass;
    }
    return particles.front()->getPosition();
}