#pragma once
#include <vector>
#include <utility>
#include <unordered_map>
#include <cstddef>

// Forward declaring RigidBody isolates compilation boundaries, 
// preventing heavy headers from bloating compile times.
class RigidBody;

/**
 * @brief A high-performance uniform spatial partitioning broadphase grid.
 * Divides 2D space into discrete, uniform square cell buckets, reducing broadphase 
 * collision check complexity from O(N^2) to near O(N) by only testing localized bodies.
 */
class SpatialGrid {
public:
    /**
     * @brief Constructs the spatial grid with a uniform cell scale.
     * @param cellSize The dimension (width and height) of a single square cell in world units.
     */
    explicit SpatialGrid(float cellSize) noexcept;

    // --- High-Performance System Rule of 5 ---
    // A spatial partitioning manager governs localized memory buffers and should never be copied.
    ~SpatialGrid() noexcept = default;
    SpatialGrid(const SpatialGrid&) = delete;
    SpatialGrid& operator=(const SpatialGrid&) = delete;
    SpatialGrid(SpatialGrid&&) noexcept = default;
    SpatialGrid& operator=(SpatialGrid&&) noexcept = default;

    /**
     * @brief Instantly flushes all active cell buckets, clearing transient pointer tracking.
     * Run this at the start of every physical update sweep.
     */
    void clear() noexcept;

    /**
     * @brief Determines a body's world-space Axis-Aligned Bounding Box (AABB) 
     * and maps raw pointers to every intersecting cell bucket.
     * @param body The rigid entity pointer to insert into the tracking index.
     */
    void insert(RigidBody* body) noexcept;

    /**
     * @brief Processes active cell buckets to extract unique, non-duplicate colliding candidate pairs.
     * Only yields pairs of bodies that inhabit the exact same or adjacent spatial cells.
     * @param potentialPairs Output vector to populate with localized broadphase candidates.
     */
    void queryPotentialPairs(std::vector<std::pair<RigidBody*, RigidBody*>>& potentialPairs) noexcept;

private:
    /**
     * @brief Custom 2D spatial coordinate hash generator.
     * Employs large prime coordinate multipliers to generate unique 1D hash indices, 
     * drastically reducing spatial mapping collisions in open-ended or infinite maps.
     */
    struct SpatialHash {
        std::size_t operator()(const std::pair<int, int>& gridPos) const noexcept {
            const std::size_t h1 = static_cast<std::size_t>(gridPos.first) * 73856093ULL;
            const std::size_t h2 = static_cast<std::size_t>(gridPos.second) * 19349663ULL;
            return h1 ^ h2;
        }
    };

    float cellSize; // Dimensional bounding width/height of a single cell division

    // Dynamic hash map indexing 2D integer cell coordinates (X, Y) to lists of overlapping bodies.
    // Memory is only allocated for cells that actively contain entities.
    std::unordered_map<std::pair<int, int>, std::vector<RigidBody*>, SpatialHash> gridBuckets;
};