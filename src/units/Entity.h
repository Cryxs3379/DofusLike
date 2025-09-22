#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include "map/Map.h"
#include "systems/Pathfinding.h"

enum class EntityType {
    Player,
    Enemy
};

enum class EntityState {
    Idle,
    Moving
};

class Entity {
public:
    Entity(sf::Vector2i startPosition, EntityType type);
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window, const Map& map);
    
    void moveTo(sf::Vector2i targetPosition, const Map& map);
    void setPosition(sf::Vector2i position);
    
    sf::Vector2i getPosition() const { return m_currentPosition; }
    bool isMoving() const { return m_state == EntityState::Moving; }
    int getRemainingPM() const { return m_remainingPM; }
    int getTotalPM() const { return m_totalPM; }
    int getRemainingPA() const { return m_remainingPA; }
    int getTotalPA() const { return m_totalPA; }
    int getHP() const { return m_hp; }
    EntityType getType() const { return m_type; }
    EntityState getState() const { return m_state; }
    
    std::vector<sf::Vector2i> getReachableTiles(const Map& map) const;
    void startTurn();
    void endTurn();
    int stepsRemainingInQueue() const { return static_cast<int>(m_movementPath.size()); }
    
    // Sistema de combate
    bool tryCastStrike(sf::Vector2i targetCell, Entity& target);
    void takeDamage(int damage);
    bool isAlive() const { return m_hp > 0; }
    bool isInRange(sf::Vector2i target, int maxRange) const;
    
    // Sistema de hechizos con alcance
    bool canCastSpell(sf::Vector2i targetCell, int minRange, int maxRange, const Map& map) const;
    std::vector<sf::Vector2i> getCastableCells(const Map& map, int minRange, int maxRange) const;
    void castSpell(sf::Vector2i targetCell, int minRange, int maxRange, const Map& map, int paCost);
    void consumePA(int amount);
    
private:
    sf::Vector2i m_currentPosition;
    sf::Vector2f m_screenPosition;
    std::vector<sf::Vector2i> m_movementPath;
    float m_movementTimer;
    static constexpr float MOVEMENT_SPEED = 0.18f; // segundos por casilla
    
    sf::CircleShape m_entityShape;
    sf::Vector2f m_targetScreenPosition;
    bool m_isMovingToTarget;
    
    // Sistema de recursos
    int m_totalPM;
    int m_remainingPM;
    int m_totalPA;
    int m_remainingPA;
    int m_hp;
    EntityType m_type;
    EntityState m_state;
    
    void updateMovement(float deltaTime);
    void updateScreenPosition(const Map& map);
    void consumePM(int amount);
};
