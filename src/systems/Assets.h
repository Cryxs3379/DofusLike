#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <optional>

class Assets {
public:
    // Obtener textura desde archivo (con cache)
    static sf::Texture* getTexture(const std::string& path);
    
    // Obtener textura vacía para inicializar sprites
    static sf::Texture* getEmptyTexture();
    
    // Limpiar cache (opcional, para liberar memoria)
    static void clearCache();
    
    // Establecer directorio raíz personalizado para assets
    static void setRoot(const std::filesystem::path& root);
    
private:
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>> s_textureCache;
    static std::unique_ptr<sf::Texture> s_emptyTexture;
    static std::optional<std::filesystem::path> s_customRoot;
    
    // Helper para intentar cargar desde múltiples rutas
    static bool tryLoadTexture(sf::Texture& texture, const std::string& relativePath, std::filesystem::path& outFullPath);
};
