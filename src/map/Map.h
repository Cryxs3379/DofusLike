#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <SFML/System.hpp>
#include "map/Isometric.h"

class Map {
public:
    static constexpr int MAP_SIZE = 15;
    static constexpr float TILE_SIZE = 40.0f;
    
    Map();
    
    void render(sf::RenderWindow& window);
    void handleMouseClick(sf::Vector2f mousePos, sf::Mouse::Button button);
    void updateHover(sf::Vector2f mousePos);
    
    bool isBlocked(int x, int y) const;
    void setBlocked(int x, int y, bool blocked);
    bool isValidPosition(int x, int y) const;
    
    sf::Vector2f getTileCenter(int x, int y) const;
    sf::Vector2i getTileFromPosition(sf::Vector2f position) const;
    
    // Métodos para carga/guardado de mapas
    bool loadFromArray(int width, int height, const std::vector<uint8_t>& blocked);
    std::vector<uint8_t> exportBlockedLinear() const;
    int getWidth() const { return MAP_SIZE; }
    int getHeight() const { return MAP_SIZE; }
    
private:
    std::vector<std::vector<bool>> m_blockedTiles;
    sf::Vector2i m_hoveredTile;
    sf::Vector2f m_offset;
    
    sf::Color getTileColor(int x, int y) const;
    void toggleTile(int x, int y);
};
