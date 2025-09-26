#pragma once
#include <SFML/System.hpp>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>
#include "map/Map.h"

// Comparador para sf::Vector2i para usar en contenedores ordenados
struct Vec2Less {
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const {
        return (a.y < b.y) || (a.y == b.y && a.x < b.x);
    }
};

// Hash para sf::Vector2i para usar en unordered_map
struct Vec2Hash {
    std::size_t operator()(const sf::Vector2i& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};

// Nodo para A* con gCost (distancia acumulada) y hCost (heurística)
struct AStarNode {
    sf::Vector2i position;
    int gCost;  // Costo real desde el inicio
    int hCost;  // Heurística (Manhattan distance al destino)
    int fCost;  // gCost + hCost (calculado automáticamente)
    
    AStarNode(sf::Vector2i pos, int g, int h) : position(pos), gCost(g), hCost(h), fCost(g + h) {}
    
    // Comparador para priority_queue (menor fCost tiene mayor prioridad)
    bool operator>(const AStarNode& other) const {
        if (fCost != other.fCost) {
            return fCost > other.fCost;
        }
        // En caso de empate, usar hCost como desempate
        return hCost > other.hCost;
    }
};

// Nodo simple para getReachableTiles (mantiene compatibilidad)
struct Node {
    sf::Vector2i position;
    int cost;
    
    Node(sf::Vector2i pos, int c) : position(pos), cost(c) {}
    
    bool operator>(const Node& other) const {
        return cost > other.cost;
    }
};

class Pathfinding {
public:
    static constexpr int MAX_MOVEMENT_POINTS = 3;
    
    static std::vector<sf::Vector2i> getReachableTiles(const Map& map, sf::Vector2i startPos, int maxCost);
    static std::vector<sf::Vector2i> getReachableTiles(const Map& map, sf::Vector2i startPos, int maxCost, const std::vector<sf::Vector2i>& excludedPositions);
    static std::vector<sf::Vector2i> findPath(const Map& map, sf::Vector2i start, sf::Vector2i end);
    
private:
    static std::vector<sf::Vector2i> getNeighbors(sf::Vector2i pos);
    static int getMovementCost(const Map& map, sf::Vector2i from, sf::Vector2i to);
    
    // Métodos específicos para A*
    static int manhattanDistance(sf::Vector2i a, sf::Vector2i b);
    static std::vector<sf::Vector2i> reconstructPath(const std::unordered_map<sf::Vector2i, sf::Vector2i, Vec2Hash>& parent, sf::Vector2i start, sf::Vector2i end);
};
