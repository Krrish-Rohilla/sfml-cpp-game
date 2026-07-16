#pragma once
#include <string_view>
#include <vector>

// Forward declarations to keep compilation tight
class LevelManager;
class RigidBody;

/**
 * @brief Handles the binary or text-based persistence of the simulation state.
 * 
 * The Serializer converts the dynamic runtime state of the LevelManager (active bodies, 
 * active springs, environment constants) into an external file and reconstructs 
 * that state deterministically.
 */
class Serializer {
public:
    // Delete constructor to prevent instantiation of a static utility class
    Serializer() = delete;

    /**
     * @brief Saves the current simulation snapshot to the specified path.
     * @param manager The authoritative LevelManager instance containing the simulation state.
     * @param filepath The target disk path for the serialized scene file.
     * @return true if serialization succeeded, false on disk/format errors.
     */
    [[nodiscard]] static bool saveLevel(const LevelManager& manager, std::string_view filepath) noexcept;

    /**
     * @brief Loads a simulation snapshot from the specified path into the provided manager.
     * @param manager The target LevelManager to overwrite with loaded state.
     * @param filepath The source disk path for the serialized scene file.
     * @return true if deserialization succeeded, false on disk/format/version errors.
     */
    [[nodiscard]] static bool loadLevel(LevelManager& manager, std::string_view filepath) noexcept;

private:
    /**
     * @brief Internal helper to resolve rigid body pointer references to array indices.
     * Essential for maintaining Spring constraint topology during save/load cycles.
     */
    static int findBodyIndex(const std::vector<RigidBody*>& bodies, const RigidBody* target) noexcept;
};