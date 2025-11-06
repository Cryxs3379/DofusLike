#include "map/Isometric.h"
#include <cmath>

namespace Isometric {
    sf::Vector2i screenToIso(sf::Vector2f screenPos, sf::Vector2f tileSize) {
        float x = (screenPos.x / tileSize.x) + (screenPos.y / tileSize.y);
        float y = (screenPos.y / tileSize.y) - (screenPos.x / tileSize.x);
        return sf::Vector2i(static_cast<int>(std::floor(x)), static_cast<int>(std::floor(y)));
    }
    
    sf::Vector2f isoToScreen(sf::Vector2i isoPos, sf::Vector2f tileSize) {
        float x = (isoPos.x - isoPos.y) * (tileSize.x * 0.5f);
        float y = (isoPos.x + isoPos.y) * (tileSize.y * 0.5f);
        return sf::Vector2f(x, y);
    }
    
    sf::ConvexShape createDiamond(sf::Vector2f tileSize, sf::Color color) {
        sf::ConvexShape diamond(4);
        diamond.setPoint(0, sf::Vector2f(tileSize.x * 0.5f, 0));
        diamond.setPoint(1, sf::Vector2f(tileSize.x, tileSize.y * 0.5f));
        diamond.setPoint(2, sf::Vector2f(tileSize.x * 0.5f, tileSize.y));
        diamond.setPoint(3, sf::Vector2f(0, tileSize.y * 0.5f));
        diamond.setFillColor(color);
        diamond.setOutlineColor(sf::Color::Black);
        diamond.setOutlineThickness(1.0f);
        return diamond;
    }
    
    bool pointInDiamond(sf::Vector2f point, sf::Vector2f diamondCenter, sf::Vector2f tileSize) {
        float dx = std::abs(point.x - diamondCenter.x);
        float dy = std::abs(point.y - diamondCenter.y);
        return (dx / (tileSize.x * 0.5f)) + (dy / (tileSize.y * 0.5f)) <= 1.0f;
    }
}
