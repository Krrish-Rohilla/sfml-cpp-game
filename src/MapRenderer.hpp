#pragma once
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

/**
 * @brief Manages the visual representation of static map geometry.
 * Batches all static terrain into a single vertex array for high-performance rendering.
 */
class MapRenderer : public sf::Drawable, public sf::Transformable {
public:
    MapRenderer() noexcept = default;
    ~MapRenderer() noexcept = default;

    // Disallow copies to prevent vertex cache duplication
    MapRenderer(const MapRenderer&) = delete;
    MapRenderer& operator=(const MapRenderer&) = delete;

    /**
     * @brief Adds geometry to the batched vertex cache.
     */
    void addGeometry(const sf::VertexArray& geometry) noexcept;

    /**
     * @brief Clears the current visual batch.
     */
    void clear() noexcept;

protected:
    /**
     * @brief Overridden sf::Drawable method for batch rendering.
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::VertexArray visualCache{sf::PrimitiveType::Triangles};
};