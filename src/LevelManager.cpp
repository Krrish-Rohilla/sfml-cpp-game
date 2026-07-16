#include "LevelManager.hpp"
#include "PhysicsSystem.hpp"
#include "SpatialGrid.hpp"
#include "Spring.hpp"
#include "RigidBody.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <algorithm>
#include <cmath>

LevelManager::LevelManager(float gravity, float airDrag, float spatialCellSize) noexcept
    : spatialGrid(spatialCellSize),
      gravity(gravity),
      airDrag(airDrag)
{
    bodies.reserve(256);
    springs.reserve(512);
}

LevelManager::~LevelManager() noexcept {
    clearLevel();
}

void LevelManager::processInput([[maybe_unused]] const sf::Event& event) noexcept {
    // Top-level input processing layer. Delegating event variations to player-controlled 
    // physics entities here keeps player mechanics decoupled from the core systems solver.
}

void LevelManager::update(float deltaTime) noexcept {
    if (deltaTime > 0.25f) deltaTime = 0.25f;

    timeAccumulator += deltaTime;

    while (timeAccumulator >= fixedTimeStep) {
        PhysicsSystem::step(bodies, springs, spatialGrid, fixedTimeStep, gravity, airDrag);
        timeAccumulator -= fixedTimeStep;
    }

    postFrameCleanup();
}

void LevelManager::render(sf::RenderWindow& window) const noexcept {
    // 1. Render all active visible Spring Constraint Networks
    if (!springs.empty()) {
        std::vector<sf::Vertex> lineVertices;
        lineVertices.reserve(springs.size() * 2);

        for (const auto& spring : springs) {
            if (!spring || !spring->visible) continue;

            lineVertices.emplace_back(spring->bodyA->getPosition(), sf::Color(200, 50, 50, 150));
            lineVertices.emplace_back(spring->bodyB->getPosition(), sf::Color(200, 50, 50, 150));
        }

        if (!lineVertices.empty()) {
            window.draw(lineVertices.data(), lineVertices.size(), sf::PrimitiveType::Lines);
        }
    }

    // 2. Render all active visible Rigid Body Geometry Profiles
    for (const auto& body : bodies) {
        if (!body || !body->isVisible()) continue;

        const sf::Vector2f position = body->getPosition();
        const float angleDegrees    = body->getRotationAngle() * 180.0f / 3.14159265f;

        if (body->getType() == ShapeType::Circle) {
            const float radius = body->getRadius();
            sf::CircleShape circle(radius);
            circle.setOrigin({radius, radius});
            circle.setPosition(position);
            circle.setRotation(sf::degrees(angleDegrees));

            if (body->isTrigger()) {
                circle.setFillColor(sf::Color(0, 255, 100, 80));
                circle.setOutlineColor(sf::Color(0, 255, 100, 200));
                circle.setOutlineThickness(1.0f);
            } else {
                circle.setFillColor(body->getInverseMass() == 0.0f ? sf::Color(80, 80, 80) : sf::Color(180, 180, 180));
            }
            window.draw(circle);
        }
        else if (body->getType() == ShapeType::Polygon) {
            const auto& localVertices = body->getLocalVertices();
            sf::ConvexShape convexPolygon(localVertices.size());

            for (std::size_t i = 0; i < localVertices.size(); ++i) {
                convexPolygon.setPoint(i, localVertices[i]);
            }
            convexPolygon.setOrigin({0.0f, 0.0f});
            convexPolygon.setPosition(position);
            convexPolygon.setRotation(sf::degrees(angleDegrees));

            if (body->isTrigger()) {
                convexPolygon.setFillColor(sf::Color(0, 100, 255, 80));
                convexPolygon.setOutlineColor(sf::Color(0, 100, 255, 200));
                convexPolygon.setOutlineThickness(1.0f);
            } else {
                convexPolygon.setFillColor(body->getInverseMass() == 0.0f ? sf::Color(60, 60, 60) : sf::Color(100, 140, 180));
            }
            window.draw(convexPolygon);
        }
    }
}

RigidBody* LevelManager::spawnCircle(sf::Vector2f position, float radius, sf::Vector2f velocity, 
                                     float mass, float restitution, float friction, bool isTrigger) noexcept 
{
    bodies.push_back(std::make_unique<RigidBody>(position, radius, velocity, mass, restitution, friction, isTrigger));
    return bodies.back().get();
}

RigidBody* LevelManager::spawnPolygon(sf::Vector2f position, const std::vector<sf::Vector2f>& vertices, 
                                      sf::Vector2f velocity, float mass, float restitution, float friction, bool isTrigger) noexcept 
{
    bodies.push_back(std::make_unique<RigidBody>(position, vertices, velocity, mass, restitution, friction, isTrigger));
    return bodies.back().get();
}

void LevelManager::addSpring(RigidBody* bodyA, RigidBody* bodyB, float restLength, float stiffness, float damping) noexcept {
    if (!bodyA || !bodyB) return;
    
    auto spring = std::make_unique<Spring>();
    spring->bodyA      = bodyA;
    spring->bodyB      = bodyB;
    spring->restLength = restLength;
    spring->stiffness  = stiffness;
    spring->damping    = damping;
    spring->visible    = true;
    
    springs.push_back(std::move(spring));
}

void LevelManager::clearLevel() noexcept {
    springs.clear();
    bodies.clear();
    timeAccumulator = 0.0f;
}

void LevelManager::postFrameCleanup() noexcept {
    springs.erase(
        std::remove_if(springs.begin(), springs.end(), [](const std::unique_ptr<Spring>& spring) noexcept {
            return spring->bodyA->isDead() || spring->bodyB->isDead();
        }), 
        springs.end()
    );

    bodies.erase(
        std::remove_if(bodies.begin(), bodies.end(), [](const std::unique_ptr<RigidBody>& body) noexcept {
            return body->isDead();
        }), 
        bodies.end()
    );
}