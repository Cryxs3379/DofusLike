#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include "map/Map.h"
#include "systems/Pathfinding.h"
#include "systems/Assets.h"
#include "systems/Animation.h"

enum class PawnState {
    Idle,
    Moving
};

class Pawn {
public:
    Pawn(sf::Vector2i startPosition);
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window, const Map& map);
    
    void moveTo(sf::Vector2i targetPosition, const Map& map);
    void setPosition(sf::Vector2i position);
    
    sf::Vector2i getPosition() const { return m_currentPosition; }
    bool isMoving() const { return m_state == PawnState::Moving; }
    int getRemainingPM() const { return m_remainingPM; }
    int getTotalPM() const { return m_totalPM; }
    PawnState getState() const { return m_state; }
    
    std::vector<sf::Vector2i> getReachableTiles(const Map& map) const;
    void endTurn();
    int stepsRemainingInQueue() const { return static_cast<int>(m_movementPath.size()); }
    
private:
    sf::Vector2i m_currentPosition;
    sf::Vector2f m_screenPosition;
    std::vector<sf::Vector2i> m_movementPath;
    float m_movementTimer;
    static constexpr float MOVEMENT_SPEED = 0.18f; // segundos por casilla
    
    sf::CircleShape m_pawnShape;
    sf::Vector2f m_targetScreenPosition;
    bool m_isMovingToTarget;
    
    // Sistema de sprites
    bool m_useSprite = false;
    sf::Sprite m_sprite{sf::Texture{}}; // Inicializar con textura vacía
    sf::Texture* m_texture = nullptr;
    Animation m_anim;
    sf::Vector2f m_spriteOffset = {0.f, 0.f};
    sf::Vector2f m_spriteScale = {1.f, 1.f};
    
    // Sistema de PM
    int m_totalPM;
    int m_remainingPM;
    PawnState m_state;
    
    void updateMovement(float deltaTime);
    void updateScreenPosition(const Map& map);
    void consumePM(int amount);
};
