#include "systems/Pathfinding.h"
#include <algorithm>
#include <iostream>

std::vector<sf::Vector2i> Pathfinding::getReachableTiles(const Map& map, sf::Vector2i startPos, int maxCost) {
    std::vector<sf::Vector2i> reachableTiles;
    std::map<sf::Vector2i, int, Vec2Less> visited;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> queue;
    
    queue.push(Node(startPos, 0));
    visited[startPos] = 0;
    
    while (!queue.empty()) {
        Node current = queue.top();
        queue.pop();
        
        if (current.cost > maxCost) continue;
        
        reachableTiles.push_back(current.position);
        
        for (auto neighbor : getNeighbors(current.position)) {
            if (!map.isValidPosition(neighbor.x, neighbor.y)) continue;
            if (map.isBlocked(neighbor.x, neighbor.y)) continue;
            
            int newCost = current.cost + getMovementCost(map, current.position, neighbor);
            
            if (newCost <= maxCost) {
                auto it = visited.find(neighbor);
                if (it == visited.end() || newCost < it->second) {
                    visited[neighbor] = newCost;
                    queue.push(Node(neighbor, newCost));
                }
            }
        }
    }
    
    return reachableTiles;
}

std::vector<sf::Vector2i> Pathfinding::getReachableTiles(const Map& map, sf::Vector2i startPos, int maxCost, const std::vector<sf::Vector2i>& excludedPositions) {
    std::vector<sf::Vector2i> reachableTiles;
    std::unordered_map<sf::Vector2i, int, Vec2Hash> visited;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> queue;
    
    queue.push(Node(startPos, 0));
    visited[startPos] = 0;
    
    while (!queue.empty()) {
        Node current = queue.top();
        queue.pop();
        
        if (current.cost > maxCost) continue;
        
        reachableTiles.push_back(current.position);
        
        for (auto neighbor : getNeighbors(current.position)) {
            if (!map.isValidPosition(neighbor.x, neighbor.y)) continue;
            if (map.isBlocked(neighbor.x, neighbor.y)) continue;
            
            // Verificar si la posición está excluida
            bool isExcluded = false;
            for (const auto& excludedPos : excludedPositions) {
                if (neighbor == excludedPos) {
                    isExcluded = true;
                    break;
                }
            }
            if (isExcluded) continue;
            
            int newCost = current.cost + getMovementCost(map, current.position, neighbor);
            
            if (newCost <= maxCost) {
                auto it = visited.find(neighbor);
                if (it == visited.end() || newCost < it->second) {
                    visited[neighbor] = newCost;
                    queue.push(Node(neighbor, newCost));
                }
            }
        }
    }
    
    return reachableTiles;
}

std::vector<sf::Vector2i> Pathfinding::findPath(const Map& map, sf::Vector2i start, sf::Vector2i end) {
    // Algoritmo A* con heurística Manhattan
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
    std::unordered_map<sf::Vector2i, sf::Vector2i, Vec2Hash> parent;
    std::unordered_map<sf::Vector2i, int, Vec2Hash> gCost;
    std::set<sf::Vector2i, Vec2Less> closedSet;
    
    // Inicializar
    int startH = manhattanDistance(start, end);
    openSet.push(AStarNode(start, 0, startH));
    gCost[start] = 0;
    
    
    while (!openSet.empty()) {
        AStarNode current = openSet.top();
        openSet.pop();
        
        // Si ya procesamos este nodo, saltarlo
        if (closedSet.find(current.position) != closedSet.end()) {
            continue;
        }
        
        closedSet.insert(current.position);
        
        // Si llegamos al destino, reconstruir el camino
        if (current.position == end) {
            return reconstructPath(parent, start, end);
        }
        
        // Explorar vecinos
        for (auto neighbor : getNeighbors(current.position)) {
            if (!map.isValidPosition(neighbor.x, neighbor.y)) continue;
            if (map.isBlocked(neighbor.x, neighbor.y)) continue;
            if (closedSet.find(neighbor) != closedSet.end()) continue;
            
            int tentativeGCost = current.gCost + getMovementCost(map, current.position, neighbor);
            
            // Si no hemos visitado este nodo o encontramos un camino mejor
            auto gIt = gCost.find(neighbor);
            if (gIt == gCost.end() || tentativeGCost < gIt->second) {
                parent[neighbor] = current.position;
                gCost[neighbor] = tentativeGCost;
                
                int hCost = manhattanDistance(neighbor, end);
                openSet.push(AStarNode(neighbor, tentativeGCost, hCost));
            }
        }
    }
    
    return {}; // No se encontró camino
}

std::vector<sf::Vector2i> Pathfinding::getNeighbors(sf::Vector2i pos) {
    return {
        {pos.x + 1, pos.y},     // Derecha
        {pos.x - 1, pos.y},     // Izquierda
        {pos.x, pos.y + 1},     // Abajo
        {pos.x, pos.y - 1}      // Arriba
    };
}

int Pathfinding::getMovementCost(const Map& map, sf::Vector2i from, sf::Vector2i to) {
    // Costo de movimiento ortogonal: 1 PM por casilla
    // Preparado para futuros costes variables por terreno
    return 1;
}

int Pathfinding::manhattanDistance(sf::Vector2i a, sf::Vector2i b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<sf::Vector2i> Pathfinding::reconstructPath(const std::unordered_map<sf::Vector2i, sf::Vector2i, Vec2Hash>& parent, sf::Vector2i start, sf::Vector2i end) {
    std::vector<sf::Vector2i> path;
    sf::Vector2i current = end;
    
    // Reconstruir el camino desde el destino hasta el inicio
    while (current != start) {
        path.push_back(current);
        auto it = parent.find(current);
        if (it == parent.end()) {
            // No debería pasar si el algoritmo está bien implementado
            return {};
        }
        current = it->second;
    }
    
    // Invertir para que vaya del inicio al destino
    std::reverse(path.begin(), path.end());
    
    // NO incluir la casilla de origen - el path debe contener solo las casillas a pisar
    return path;
}
