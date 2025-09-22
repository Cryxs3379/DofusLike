#include "systems/Pathfinding.h"
#include <algorithm>

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

std::vector<sf::Vector2i> Pathfinding::findPath(const Map& map, sf::Vector2i start, sf::Vector2i end) {
    std::map<sf::Vector2i, sf::Vector2i, Vec2Less> parent;
    std::queue<sf::Vector2i> queue;
    std::set<sf::Vector2i, Vec2Less> visited;
    
    queue.push(start);
    visited.insert(start);
    
    while (!queue.empty()) {
        sf::Vector2i current = queue.front();
        queue.pop();
        
        if (current == end) {
            // Reconstruir el camino
            std::vector<sf::Vector2i> path;
            sf::Vector2i node = end;
            
            while (node != start) {
                path.push_back(node);
                node = parent[node];
            }
            path.push_back(start);
            
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        for (auto neighbor : getNeighbors(current)) {
            if (!map.isValidPosition(neighbor.x, neighbor.y)) continue;
            if (map.isBlocked(neighbor.x, neighbor.y)) continue;
            if (visited.find(neighbor) != visited.end()) continue;
            
            visited.insert(neighbor);
            parent[neighbor] = current;
            queue.push(neighbor);
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
    return 1;
}
