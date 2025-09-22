#include "systems/Assets.h"
#include <iostream>
#include <filesystem>

std::unordered_map<std::string, std::unique_ptr<sf::Texture>> Assets::s_textureCache;
std::unique_ptr<sf::Texture> Assets::s_emptyTexture;
static bool s_firstLoad = true;

sf::Texture* Assets::getTexture(const std::string& path) {
    // Imprimir current_path una sola vez
    if (s_firstLoad) {
        std::cout << "current_path=" << std::filesystem::current_path() << std::endl;
        s_firstLoad = false;
    }
    
    // Buscar en cache
    auto it = s_textureCache.find(path);
    if (it != s_textureCache.end()) {
        return it->second.get();
    }
    
    // Cargar nueva textura
    std::cout << "Loading texture: " << path << " ... ";
    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromFile(path)) {
        sf::Vector2u size = texture->getSize();
        std::cout << "OK size=" << size.x << "x" << size.y << std::endl;
        sf::Texture* ptr = texture.get();
        s_textureCache[path] = std::move(texture);
        return ptr;
    } else {
        std::cout << "FAIL" << std::endl;
        return nullptr;
    }
}

sf::Texture* Assets::getEmptyTexture() {
    if (!s_emptyTexture) {
        s_emptyTexture = std::make_unique<sf::Texture>();
        sf::Image img({1, 1}, sf::Color::Transparent);
        (void)s_emptyTexture->loadFromImage(img);
    }
    return s_emptyTexture.get();
}

void Assets::clearCache() {
    s_textureCache.clear();
}
