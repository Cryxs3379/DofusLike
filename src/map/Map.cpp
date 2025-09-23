#include "map/Map.h"
#include <algorithm>
#include <iostream>

Map::Map() : m_blockedTiles(MAP_SIZE, std::vector<bool>(MAP_SIZE, false)), 
             m_hoveredTile(-1, -1) {
    // Offset inicial; se recalcula al aplicar letterboxing para centrar
    m_offset = sf::Vector2f(0.0f, 0.0f);
}

void Map::render(sf::RenderWindow& window) {
    for (int y = 0; y < MAP_SIZE; ++y) {
        for (int x = 0; x < MAP_SIZE; ++x) {
            sf::Vector2f screenPos = Isometric::isoToScreen(sf::Vector2i(x, y), sf::Vector2f(TILE_SIZE, TILE_SIZE));
            screenPos += m_offset;
            
            sf::Color tileColor = getTileColor(x, y);
            
            // Resaltar la loseta bajo el cursor
            if (x == m_hoveredTile.x && y == m_hoveredTile.y) {
                tileColor = sf::Color::Yellow;
            }
            
            auto diamond = Isometric::createDiamond(sf::Vector2f(TILE_SIZE, TILE_SIZE), tileColor);
            diamond.setPosition(screenPos);
            window.draw(diamond);
        }
    }
}

void Map::handleMouseClick(sf::Vector2f mousePos, sf::Mouse::Button button) {
    sf::Vector2i tilePos = getTileFromPosition(mousePos);
    
    if (isValidPosition(tilePos.x, tilePos.y)) {
        if (button == sf::Mouse::Button::Right) {
            toggleTile(tilePos.x, tilePos.y);
        }
    }
}

void Map::updateHover(sf::Vector2f mousePos) {
    m_hoveredTile = getTileFromPosition(mousePos);
}

bool Map::isBlocked(int x, int y) const {
    if (!isValidPosition(x, y)) return true;
    return m_blockedTiles[y][x];
}

void Map::setBlocked(int x, int y, bool blocked) {
    if (isValidPosition(x, y)) {
        m_blockedTiles[y][x] = blocked;
    }
}

bool Map::isValidPosition(int x, int y) const {
    return x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE;
}

sf::Vector2f Map::getTileCenter(int x, int y) const {
    sf::Vector2f screenPos = Isometric::isoToScreen(sf::Vector2i(x, y), sf::Vector2f(TILE_SIZE, TILE_SIZE));
    screenPos += m_offset;
    screenPos.x += TILE_SIZE * 0.5f;
    screenPos.y += TILE_SIZE * 0.5f;
    return screenPos;
}

sf::Vector2f Map::getTileTopLeft(int x, int y) const {
    sf::Vector2f screenPos = Isometric::isoToScreen(sf::Vector2i(x, y), sf::Vector2f(TILE_SIZE, TILE_SIZE));
    screenPos += m_offset;
    return screenPos;
}

void Map::setCenteredOffset(sf::Vector2f viewSize) {
    // Centrar el rombo del mapa dentro de la vista virtual 1280x720
    // El tamaño del rombo en píxeles: ancho = MAP_SIZE*TILE_SIZE, alto = MAP_SIZE*TILE_SIZE
    const float mapW = MAP_SIZE * TILE_SIZE;
    const float mapH = MAP_SIZE * TILE_SIZE;
    m_offset.x = (viewSize.x - mapW) * 0.5f;
    m_offset.y = (viewSize.y - mapH) * 0.5f;
}

sf::Vector2i Map::getTileFromPosition(sf::Vector2f position) const {
    sf::Vector2f relativePos = position - m_offset;
    return Isometric::screenToIso(relativePos, sf::Vector2f(TILE_SIZE, TILE_SIZE));
}

sf::Color Map::getTileColor(int x, int y) const {
    if (isBlocked(x, y)) {
        return sf::Color::Red;
    }
    return sf::Color::Green;
}

void Map::toggleTile(int x, int y) {
    if (isValidPosition(x, y)) {
        m_blockedTiles[y][x] = !m_blockedTiles[y][x];
    }
}

bool Map::loadFromArray(int width, int height, const std::vector<uint8_t>& blocked) {
    // Validar dimensiones
    if (width != MAP_SIZE || height != MAP_SIZE) {
        std::cout << "Error: Dimensiones del mapa no coinciden. Esperado: " << MAP_SIZE 
                  << "x" << MAP_SIZE << ", Obtenido: " << width << "x" << height << std::endl;
        return false;
    }
    
    if (static_cast<int>(blocked.size()) != width * height) {
        std::cout << "Error: Tamaño del array blocked incorrecto. Esperado: " << width * height 
                  << ", Obtenido: " << blocked.size() << std::endl;
        return false;
    }
    
    // Cargar datos
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;
            m_blockedTiles[y][x] = (blocked[index] != 0);
        }
    }
    
    std::cout << "Mapa cargado desde array: " << width << "x" << height << " con " 
              << std::count(blocked.begin(), blocked.end(), 1) << " casillas bloqueadas" << std::endl;
    
    return true;
}

std::vector<uint8_t> Map::exportBlockedLinear() const {
    std::vector<uint8_t> result;
    result.reserve(MAP_SIZE * MAP_SIZE);
    
    for (int y = 0; y < MAP_SIZE; ++y) {
        for (int x = 0; x < MAP_SIZE; ++x) {
            result.push_back(m_blockedTiles[y][x] ? 1 : 0);
        }
    }
    
    return result;
}
