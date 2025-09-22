#include "map/Map.h"
#include <algorithm>

Map::Map() : m_blockedTiles(MAP_SIZE, std::vector<bool>(MAP_SIZE, false)), 
             m_hoveredTile(-1, -1) {
    // Centrar el mapa en la pantalla
    m_offset = sf::Vector2f(400.0f, 300.0f);
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
