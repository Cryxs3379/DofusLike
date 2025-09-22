#include "units/Pawn.h"
#include <algorithm>

Pawn::Pawn(sf::Vector2i startPosition) 
    : m_currentPosition(startPosition), 
      m_screenPosition(0, 0),
      m_movementTimer(0.0f),
      m_isMovingToTarget(false),
      m_totalPM(3),
      m_remainingPM(3),
      m_state(PawnState::Idle) {
    
    m_pawnShape.setRadius(8.0f);
    m_pawnShape.setFillColor(sf::Color::Blue);
    m_pawnShape.setOutlineColor(sf::Color::White);
    m_pawnShape.setOutlineThickness(2.0f);
    m_pawnShape.setOrigin({8.0f, 8.0f});
}

void Pawn::update(float deltaTime) {
    updateMovement(deltaTime);
}

void Pawn::render(sf::RenderWindow& window, const Map& map) {
    updateScreenPosition(map);
    m_pawnShape.setPosition(m_screenPosition);
    window.draw(m_pawnShape);
}

void Pawn::moveTo(sf::Vector2i targetPosition, const Map& map) {
    if (targetPosition == m_currentPosition || m_state == PawnState::Moving) return;
    
    std::vector<sf::Vector2i> path = Pathfinding::findPath(map, m_currentPosition, targetPosition);
    if (!path.empty()) {
        // Eliminar el primer nodo si es igual a la posición actual
        if (!path.empty() && path.front() == m_currentPosition) {
            path.erase(path.begin());
        }
        
        // Recortar el camino según los PM disponibles
        int maxSteps = m_remainingPM;
        if (static_cast<int>(path.size()) > maxSteps) {
            path.resize(maxSteps);
        }
        
        if (!path.empty()) {
            m_movementPath = path;
            m_movementTimer = 0.0f;
            m_isMovingToTarget = true;
            m_state = PawnState::Moving;
        }
    }
}

void Pawn::setPosition(sf::Vector2i position) {
    m_currentPosition = position;
    m_movementPath.clear();
    m_isMovingToTarget = false;
    m_state = PawnState::Idle;
}

std::vector<sf::Vector2i> Pawn::getReachableTiles(const Map& map) const {
    return Pathfinding::getReachableTiles(map, m_currentPosition, m_remainingPM);
}

void Pawn::endTurn() {
    m_remainingPM = m_totalPM;
    m_state = PawnState::Idle;
}

void Pawn::updateMovement(float deltaTime) {
    if (m_movementPath.empty()) {
        m_isMovingToTarget = false;
        m_state = PawnState::Idle;
        return;
    }
    
    m_movementTimer += deltaTime;
    
    if (m_movementTimer >= MOVEMENT_SPEED) {
        m_movementTimer = 0.0f;
        
        if (!m_movementPath.empty()) {
            m_currentPosition = m_movementPath.front();
            m_movementPath.erase(m_movementPath.begin());
            consumePM(1); // Descontar 1 PM por cada paso
        }
    }
}

void Pawn::updateScreenPosition(const Map& map) {
    sf::Vector2f targetPos = map.getTileCenter(m_currentPosition.x, m_currentPosition.y);
    
    if (m_isMovingToTarget && !m_movementPath.empty()) {
        // Interpolar entre la posición actual y la siguiente en el camino
        sf::Vector2i nextPos = m_movementPath.front();
        sf::Vector2f nextScreenPos = map.getTileCenter(nextPos.x, nextPos.y);
        
        float progress = m_movementTimer / MOVEMENT_SPEED;
        m_screenPosition = m_screenPosition + (nextScreenPos - m_screenPosition) * progress;
    } else {
        m_screenPosition = targetPos;
    }
}

void Pawn::consumePM(int amount) {
    m_remainingPM = std::max(0, m_remainingPM - amount);
}
