#include "Serializer.hpp"
#include "LevelManager.hpp"
#include "RigidBody.hpp"
#include "Spring.hpp"
#include <fstream>
#include <vector>
#include <memory>

bool Serializer::saveLevel(const LevelManager& manager, std::string_view filepath) noexcept {
    std::ofstream ofs(filepath.data(), std::ios::binary);
    if (!ofs.is_open()) return false;

    // Accessors assumed to be added to LevelManager for serialization access
    const auto& bodies = manager.getBodies();
    const auto& springs = manager.getSprings();

    // 1. Serialize Body Count
    const size_t bodyCount = bodies.size();
    ofs.write(reinterpret_cast<const char*>(&bodyCount), sizeof(bodyCount));

    // 2. Serialize Bodies
    for (const auto& body : bodies) {
        // We write the raw POD state of the body
        // Note: In production, ensure RigidBody has a serialized state struct
        // Here we write the critical state members
        auto type = body->getType();
        auto pos = body->getPosition();
        auto vel = body->getVelocity();
        float mass = body->getMass();
        float rest = body->getRestitution();
        float fric = body->getFriction();
        bool trig = body->isTrigger();

        ofs.write(reinterpret_cast<const char*>(&type), sizeof(type));
        ofs.write(reinterpret_cast<const char*>(&pos), sizeof(pos));
        ofs.write(reinterpret_cast<const char*>(&vel), sizeof(vel));
        ofs.write(reinterpret_cast<const char*>(&mass), sizeof(mass));
        ofs.write(reinterpret_cast<const char*>(&rest), sizeof(rest));
        ofs.write(reinterpret_cast<const char*>(&fric), sizeof(fric));
        ofs.write(reinterpret_cast<const char*>(&trig), sizeof(trig));
    }

    // 3. Serialize Spring Count
    const size_t springCount = springs.size();
    ofs.write(reinterpret_cast<const char*>(&springCount), sizeof(springCount));

    // 4. Serialize Springs
    // We must map pointers to indices for the save file
    std::vector<RigidBody*> bodyPtrs;
    bodyPtrs.reserve(bodies.size());
    for(const auto& b : bodies) bodyPtrs.push_back(b.get());

    for (const auto& spring : springs) {
        int idxA = findBodyIndex(bodyPtrs, spring->bodyA);
        int idxB = findBodyIndex(bodyPtrs, spring->bodyB);
        
        ofs.write(reinterpret_cast<const char*>(&idxA), sizeof(idxA));
        ofs.write(reinterpret_cast<const char*>(&idxB), sizeof(idxB));
        ofs.write(reinterpret_cast<const char*>(&spring->restLength), sizeof(spring->restLength));
        ofs.write(reinterpret_cast<const char*>(&spring->stiffness), sizeof(spring->stiffness));
        ofs.write(reinterpret_cast<const char*>(&spring->damping), sizeof(spring->damping));
        ofs.write(reinterpret_cast<const char*>(&spring->visible), sizeof(spring->visible));
    }

    return true;
}

bool Serializer::loadLevel(LevelManager& manager, std::string_view filepath) noexcept {
    std::ifstream ifs(filepath.data(), std::ios::binary);
    if (!ifs.is_open()) return false;

    manager.clearLevel();

    // 1. Read Body Count
    size_t bodyCount = 0;
    ifs.read(reinterpret_cast<char*>(&bodyCount), sizeof(bodyCount));

    // 2. Read Bodies and Re-spawn
    std::vector<RigidBody*> loadedBodies;
    loadedBodies.reserve(bodyCount);

    for (size_t i = 0; i < bodyCount; ++i) {
        ShapeType type;
        sf::Vector2f pos, vel;
        float mass, rest, fric;
        bool trig;

        ifs.read(reinterpret_cast<char*>(&type), sizeof(type));
        ifs.read(reinterpret_cast<char*>(&pos), sizeof(pos));
        ifs.read(reinterpret_cast<char*>(&vel), sizeof(vel));
        ifs.read(reinterpret_cast<char*>(&mass), sizeof(mass));
        ifs.read(reinterpret_cast<char*>(&rest), sizeof(rest));
        ifs.read(reinterpret_cast<char*>(&fric), sizeof(fric));
        ifs.read(reinterpret_cast<char*>(&trig), sizeof(trig));

        // Use the LevelManager factory to ensure proper ownership
        if (type == ShapeType::Circle) {
            loadedBodies.push_back(manager.spawnCircle(pos, 20.0f, vel, mass, rest, fric, trig));
        } else {
            // Simplified polygon spawn
            loadedBodies.push_back(manager.spawnPolygon(pos, {{0,0}, {10,0}, {0,10}}, vel, mass, rest, fric, trig));
        }
    }

    // 3. Read Spring Count
    size_t springCount = 0;
    ifs.read(reinterpret_cast<char*>(&springCount), sizeof(springCount));

    // 4. Read Springs and Re-link
    for (size_t i = 0; i < springCount; ++i) {
        int idxA, idxB;
        float rest, stiff, damp;
        bool vis;

        ifs.read(reinterpret_cast<char*>(&idxA), sizeof(idxA));
        ifs.read(reinterpret_cast<char*>(&idxB), sizeof(idxB));
        ifs.read(reinterpret_cast<char*>(&rest), sizeof(rest));
        ifs.read(reinterpret_cast<char*>(&stiff), sizeof(stiff));
        ifs.read(reinterpret_cast<char*>(&damp), sizeof(damp));
        ifs.read(reinterpret_cast<char*>(&vis), sizeof(vis));

        if (idxA >= 0 && idxB >= 0) {
            manager.addSpring(loadedBodies[idxA], loadedBodies[idxB], rest, stiff, damp);
        }
    }

    return true;
}

int Serializer::findBodyIndex(const std::vector<RigidBody*>& bodies, const RigidBody* target) noexcept {
    for (size_t i = 0; i < bodies.size(); ++i) {
        if (bodies[i] == target) return static_cast<int>(i);
    }
    return -1;
}