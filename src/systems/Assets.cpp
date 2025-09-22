#include "systems/Assets.h"
#include <iostream>

std::unordered_map<std::string, std::unique_ptr<sf::Texture>> Assets::s_textureCache;

sf::Texture* Assets::getTexture(const std::string& path) {
    // Buscar en cache
    auto it = s_textureCache.find(path);
    if (it != s_textureCache.end()) {
        return it->second.get();
    }
    
    // Cargar nueva textura
    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromFile(path)) {
        std::cout << "Textura cargada: " << path << std::endl;
        sf::Texture* ptr = texture.get();
        s_textureCache[path] = std::move(texture);
        return ptr;
    } else {
        std::cout << "Sprite no encontrado: " << path << ", usando fallback" << std::endl;
        return nullptr;
    }
}

void Assets::clearCache() {
    s_textureCache.clear();
}
