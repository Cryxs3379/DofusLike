#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace Isometric {
    // Convierte coordenadas de pantalla a coordenadas isométricas
    sf::Vector2i screenToIso(sf::Vector2f screenPos, sf::Vector2f tileSize);
    
    // Convierte coordenadas isométricas a coordenadas de pantalla
    sf::Vector2f isoToScreen(sf::Vector2i isoPos, sf::Vector2f tileSize);
    
    // Crea un rombo isométrico
    sf::ConvexShape createDiamond(sf::Vector2f tileSize, sf::Color color);
    
    // Verifica si un punto está dentro de un rombo
    bool pointInDiamond(sf::Vector2f point, sf::Vector2f diamondCenter, sf::Vector2f tileSize);
}
