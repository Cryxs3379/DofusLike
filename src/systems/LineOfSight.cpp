#include "systems/LineOfSight.h"
#include <algorithm>
#include <cmath>

bool LineOfSight::hasLineOfSight(const Map& map, sf::Vector2i from, sf::Vector2i to) {
    // Si es la misma celda, siempre hay LoS
    if (from == to) return true;
    
    // Obtener las celdas del raycast
    std::vector<sf::Vector2i> raycastCells = getRaycastCells(from, to);
    
    // Verificar cada celda intermedia (excluyendo la celda origen)
    for (size_t i = 1; i < raycastCells.size(); ++i) {
        sf::Vector2i cell = raycastCells[i];
        
        // Si es la celda objetivo, no la consideramos bloqueante para LoS
        if (cell == to) continue;
        
        // Si la celda intermedia está bloqueada, no hay LoS
        if (map.isBlocked(cell.x, cell.y)) {
            return false;
        }
    }
    
    return true;
}

std::vector<sf::Vector2i> LineOfSight::computeCastableCells(const Map& map, sf::Vector2i from, int minRange, int maxRange, bool requireLoS) {
    std::vector<sf::Vector2i> castableCells;
    
    // Iterar sobre todas las celdas del mapa
    for (int y = 0; y < Map::MAP_SIZE; ++y) {
        for (int x = 0; x < Map::MAP_SIZE; ++x) {
            sf::Vector2i target(x, y);
            
            // Verificar rango
            if (!isInRange(from, target, minRange, maxRange)) {
                continue;
            }
            
            // Verificar LoS si es requerido
            if (requireLoS && !hasLineOfSight(map, from, target)) {
                continue;
            }
            
            castableCells.push_back(target);
        }
    }
    
    return castableCells;
}

int LineOfSight::manhattanDistance(sf::Vector2i from, sf::Vector2i to) {
    return std::abs(to.x - from.x) + std::abs(to.y - from.y);
}

bool LineOfSight::isInRange(sf::Vector2i from, sf::Vector2i to, int minRange, int maxRange) {
    int distance = manhattanDistance(from, to);
    return distance >= minRange && distance <= maxRange;
}

std::vector<sf::Vector2i> LineOfSight::getRaycastCells(sf::Vector2i from, sf::Vector2i to) {
    std::vector<sf::Vector2i> cells;
    
    int dx = std::abs(to.x - from.x);
    int dy = std::abs(to.y - from.y);
    
    int x = from.x;
    int y = from.y;
    
    int xStep = (from.x < to.x) ? 1 : -1;
    int yStep = (from.y < to.y) ? 1 : -1;
    
    int error = dx - dy;
    
    cells.push_back(sf::Vector2i(x, y));
    
    while (x != to.x || y != to.y) {
        int error2 = 2 * error;
        
        if (error2 > -dy) {
            error -= dy;
            x += xStep;
        }
        
        if (error2 < dx) {
            error += dx;
            y += yStep;
        }
        
        cells.push_back(sf::Vector2i(x, y));
    }
    
    return cells;
}
