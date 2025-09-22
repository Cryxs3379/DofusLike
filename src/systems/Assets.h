#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <memory>

class Assets {
public:
    // Obtener textura desde archivo (con cache)
    static sf::Texture* getTexture(const std::string& path);
    
    // Obtener textura vacía para inicializar sprites
    static sf::Texture* getEmptyTexture();
    
    // Limpiar cache (opcional, para liberar memoria)
    static void clearCache();
    
private:
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>> s_textureCache;
    static std::unique_ptr<sf::Texture> s_emptyTexture;
};
