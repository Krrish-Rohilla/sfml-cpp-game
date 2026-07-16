#include "MapRenderer.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

void MapRenderer::addGeometry(const sf::VertexArray& geometry) noexcept {
    // Append the new geometry to our persistent batch
    const std::size_t startCount = visualCache.getVertexCount();
    visualCache.resize(startCount + geometry.getVertexCount());
    
    for (std::size_t i = 0; i < geometry.getVertexCount(); ++i) {
        visualCache[startCount + i] = geometry[i];
    }
}

void MapRenderer::clear() noexcept {
    visualCache.clear();
}

void MapRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Apply this object's transform to the draw states
    states.transform *= getTransform();
    target.draw(visualCache, states);
}