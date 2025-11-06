#include "systems/Assets.h"
#include <iostream>
#include <filesystem>
#include <cstdlib>

std::unordered_map<std::string, std::unique_ptr<sf::Texture>> Assets::s_textureCache;
std::unique_ptr<sf::Texture> Assets::s_emptyTexture;
std::optional<std::filesystem::path> Assets::s_customRoot;
static bool s_firstLoad = true;

void Assets::setRoot(const std::filesystem::path& root) {
    s_customRoot = root;
    std::cout << "[Assets] Custom root set to: " << root << std::endl;
}

bool Assets::tryLoadTexture(sf::Texture& texture, const std::string& relativePath, std::filesystem::path& outFullPath) {
    namespace fs = std::filesystem;
    std::vector<fs::path> searchPaths;
    
    // 1. Prioridad a variable de entorno ASSET_ROOT
    const char* envRoot = std::getenv("ASSET_ROOT");
    if (envRoot && envRoot[0] != '\0') {
        searchPaths.push_back(fs::path(envRoot));
    }
    
    // 2. Root personalizado si fue establecido
    if (s_customRoot.has_value()) {
        searchPaths.push_back(s_customRoot.value());
    }
    
    // 3. Directorio de trabajo actual
    searchPaths.push_back(fs::current_path());
    
    // 4. Padre del directorio actual
    searchPaths.push_back(fs::current_path() / "..");
    
    // 5. Dos niveles arriba
    searchPaths.push_back(fs::current_path() / ".." / "..");
    
    // 6. Subdirectorio bin
    searchPaths.push_back(fs::current_path() / "bin");
    
    // 7. Subdirectorio build
    searchPaths.push_back(fs::current_path() / "build");
    
    // 8. build/Debug (común en Visual Studio)
    searchPaths.push_back(fs::current_path() / "build" / "Debug");
    
    // Intentar cargar desde cada ruta
    for (const auto& basePath : searchPaths) {
        fs::path fullPath = basePath / relativePath;
        
        // Normalizar la ruta para eliminar .. y hacer absoluta
        std::error_code ec;
        fs::path normalizedPath = fs::absolute(fullPath, ec);
        if (ec) {
            continue; // Si falla la normalización, probar siguiente
        }
        
        // Verificar si el archivo existe
        if (fs::exists(normalizedPath, ec) && !ec) {
            // Intentar cargar
            if (texture.loadFromFile(normalizedPath.string())) {
                outFullPath = normalizedPath;
                return true;
            }
        }
    }
    
    return false;
}

sf::Texture* Assets::getTexture(const std::string& path) {
    // Imprimir current_path una sola vez
    if (s_firstLoad) {
        std::cout << "[Assets] current_path=" << std::filesystem::current_path() << std::endl;
        s_firstLoad = false;
    }
    
    // Buscar en cache
    auto it = s_textureCache.find(path);
    if (it != s_textureCache.end()) {
        return it->second.get();
    }
    
    // Cargar nueva textura con búsqueda robusta
    std::cout << "[Assets] Loading texture: " << path << " ... ";
    auto texture = std::make_unique<sf::Texture>();
    std::filesystem::path fullPath;
    
    if (tryLoadTexture(*texture, path, fullPath)) {
        sf::Vector2u size = texture->getSize();
        std::cout << "OK (found at: " << fullPath << ", size=" << size.x << "x" << size.y << ")" << std::endl;
        sf::Texture* ptr = texture.get();
        s_textureCache[path] = std::move(texture);
        return ptr;
    } else {
        std::cout << "FAIL (searched from " << std::filesystem::current_path() << " and parent directories)" << std::endl;
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
