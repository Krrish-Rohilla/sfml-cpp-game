#include "SpatialGrid.hpp"
#include "RigidBody.hpp"
#include <cmath>
#include <algorithm>

SpatialGrid::SpatialGrid(float cellSize) noexcept 
    : cellSize(cellSize) 
{
    // Enforce a strict minimum boundary to prevent division-by-zero disasters
    if (this->cellSize <= 0.0f) {
        this->cellSize = 128.0f; 
    }
}

void SpatialGrid::clear() noexcept {
    gridBuckets.clear();
}

void SpatialGrid::insert(RigidBody* body) noexcept {
    if (!body) return;

    // Extract absolute geometric boundaries using the pre-calculated bounding radius
    const sf::Vector2f position = body->getPosition();
    const float radius = body->getRadius();

    const float minX = position.x - radius;
    const float maxX = position.x + radius;
    const float minY = position.y - radius;
    const float maxY = position.y + radius;

    // Math Masterclass: std::floor ensures negative coordinates map uniformly,
    // permanently resolving the origin mirroring bug caused by raw integer truncation.
    const int startX = static_cast<int>(std::floor(minX / cellSize));
    const int endX   = static_cast<int>(std::floor(maxX / cellSize));
    const int startY = static_cast<int>(std::floor(minY / cellSize));
    const int endY   = static_cast<int>(std::floor(maxY / cellSize));

    // Map the entity pointer to every unique cellular bucket it structurally overlaps
    for (int x = startX; x <= endX; ++x) {
        for (int y = startY; y <= endY; ++y) {
            gridBuckets[{x, y}].push_back(body);
        }
    }
}

void SpatialGrid::queryPotentialPairs(std::vector<std::pair<RigidBody*, RigidBody*>>& potentialPairs) noexcept {
    // Estimate initial capacity to minimize dynamic array reallocations mid-sweep
    potentialPairs.clear();
    
    // Pass 1: Localized Cell Harvesting
    for (const auto& [cellCoords, bucket] : gridBuckets) {
        const std::size_t bucketSize = bucket.size();
        if (bucketSize < 2) continue; // No collisions possible with isolated entities

        for (std::size_t i = 0; i < bucketSize; ++i) {
            for (std::size_t j = i + 1; j < bucketSize; ++j) {
                RigidBody* a = bucket[i];
                RigidBody* b = bucket[j];

                // Performance Guard: Instantly filter out static-versus-static configurations
                if (a->getInverseMass() == 0.0f && b->getInverseMass() == 0.0f) continue;

                // Enforce a unique memory address sorting constraint to ensure consistent sorting order
                if (a > b) {
                    std::swap(a, b);
                }
                
                potentialPairs.push_back({a, b});
            }
        }
    }

    // Pass 2: Linear Cache-Friendly Duplicate Suppression
    // Since objects can span across multiple neighbor cells, they will generate duplicate pairs.
    // Sorting a flat contiguous vector keeps data strictly bounded inside L1/L2 cache lines.
    std::sort(potentialPairs.begin(), potentialPairs.end(), 
        [](const std::pair<RigidBody*, RigidBody*>& lhs, const std::pair<RigidBody*, RigidBody*>& rhs) noexcept {
            if (lhs.first != rhs.first) return lhs.first < rhs.first;
            return lhs.second < rhs.second;
        }
    );

    // Wipe out duplicate structural references in a single linear sweep
    potentialPairs.erase(
        std::unique(potentialPairs.begin(), potentialPairs.end()), 
        potentialPairs.end()
    );
}