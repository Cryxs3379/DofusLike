#pragma once
#include <SFML/System.hpp>
#include <vector>
#include "map/Map.h"

class LineOfSight {
public:
    // Verifica si hay línea de visión entre dos celdas
    static bool hasLineOfSight(const Map& map, sf::Vector2i from, sf::Vector2i to);
    
    // Calcula todas las celdas casteables desde una posición
    static std::vector<sf::Vector2i> computeCastableCells(const Map& map, sf::Vector2i from, int minRange, int maxRange, bool requireLoS = true);
    
    // Calcula la distancia Manhattan entre dos celdas
    static int manhattanDistance(sf::Vector2i from, sf::Vector2i to);
    
    // Verifica si una celda está dentro del rango especificado
    static bool isInRange(sf::Vector2i from, sf::Vector2i to, int minRange, int maxRange);
    
private:
    // Implementa el algoritmo de raycast tipo Bresenham
    static std::vector<sf::Vector2i> getRaycastCells(sf::Vector2i from, sf::Vector2i to);
};
