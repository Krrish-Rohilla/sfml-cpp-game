#pragma once
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <string_view>
#include <filesystem>
#include <iostream>

class AssetManager {
public:
    AssetManager() = delete;

    [[nodiscard]] static sf::Texture& getTexture(std::string_view path) noexcept {
        auto& reg = get();
        std::string pathStr(path);
        
        auto it = reg.textures.find(pathStr);
        if (it != reg.textures.end()) return *it->second;

        auto tex = std::make_unique<sf::Texture>();
        // SFML 3: loadFromFile works with std::filesystem::path
        if (!tex->loadFromFile(std::filesystem::path(pathStr))) { 
            std::cerr << "AssetManager: Failed to load texture: " << path << "\n";
            return getFallbackTexture();
        }

        auto& ref = *tex;
        reg.textures[pathStr] = std::move(tex);
        return ref;
    }

    [[nodiscard]] static sf::Font& getFont(std::string_view path) noexcept {
        auto& reg = get();
        std::string pathStr(path);

        auto it = reg.fonts.find(pathStr);
        if (it != reg.fonts.end()) return *it->second;

        auto font = std::make_unique<sf::Font>();
        // SFML 3: Use openFromFile for fonts
        if (!font->openFromFile(std::filesystem::path(pathStr))) {
            std::cerr << "AssetManager: Failed to load font: " << path << "\n";
            return getFallbackFont();
        }

        auto& ref = *font;
        reg.fonts[pathStr] = std::move(font);
        return ref;
    }

private:
    struct Registry {
        std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
        std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
        sf::Texture fallbackTexture;
        sf::Font fallbackFont;
        bool fallbackInitialized = false;
    };

    static Registry& get() noexcept {
        static Registry instance;
        return instance;
    }

    static sf::Texture& getFallbackTexture() noexcept {
        auto& reg = get();
        if (!reg.fallbackInitialized) {
            // SFML 3 Fix: Use constructor instead of .create()
            sf::Image img({16, 16}, sf::Color::Magenta);
            reg.fallbackTexture.loadFromImage(img);
            reg.fallbackInitialized = true;
        }
        return reg.fallbackTexture;
    }

    static sf::Font& getFallbackFont() noexcept {
        return get().fallbackFont;
    }
};