#pragma once
#include <SFML/System.hpp>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include "map/Map.h"

// Comparador para sf::Vector2i para usar en contenedores ordenados
struct Vec2Less {
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const {
        return (a.y < b.y) || (a.y == b.y && a.x < b.x);
    }
};

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
    static std::vector<sf::Vector2i> findPath(const Map& map, sf::Vector2i start, sf::Vector2i end);
    
private:
    static std::vector<sf::Vector2i> getNeighbors(sf::Vector2i pos);
    static int getMovementCost(const Map& map, sf::Vector2i from, sf::Vector2i to);
};
