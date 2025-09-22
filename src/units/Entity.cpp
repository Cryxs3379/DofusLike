#include "units/Entity.h"
#include "systems/LineOfSight.h"
#include <algorithm>
#include <iostream>

Entity::Entity(sf::Vector2i startPosition, EntityType type) 
    : m_currentPosition(startPosition), 
      m_screenPosition(0, 0),
      m_movementTimer(0.0f),
      m_isMovingToTarget(false),
      m_totalPM(3),
      m_remainingPM(3),
      m_totalPA(6),
      m_remainingPA(6),
      m_hp(100),
      m_type(type),
      m_state(EntityState::Idle) {
    
    m_entityShape.setRadius(8.0f);
    m_entityShape.setFillColor(type == EntityType::Player ? sf::Color::Blue : sf::Color::Red);
    m_entityShape.setOutlineColor(sf::Color::White);
    m_entityShape.setOutlineThickness(2.0f);
    m_entityShape.setOrigin({8.0f, 8.0f});
}

void Entity::update(float deltaTime) {
    updateMovement(deltaTime);
}

void Entity::render(sf::RenderWindow& window, const Map& map) {
    updateScreenPosition(map);
    m_entityShape.setPosition(m_screenPosition);
    window.draw(m_entityShape);
}

void Entity::moveTo(sf::Vector2i targetPosition, const Map& map) {
    if (targetPosition == m_currentPosition || m_state == EntityState::Moving) return;
    
    std::cout << "=== MOVIMIENTO ===" << std::endl;
    std::cout << "Posición actual: (" << m_currentPosition.x << "," << m_currentPosition.y << ")" << std::endl;
    std::cout << "Objetivo: (" << targetPosition.x << "," << targetPosition.y << ")" << std::endl;
    std::cout << "PM disponibles: " << m_remainingPM << std::endl;
    
    std::vector<sf::Vector2i> path = Pathfinding::findPath(map, m_currentPosition, targetPosition);
    std::cout << "Camino encontrado: " << path.size() << " pasos" << std::endl;
    
    if (!path.empty()) {
        // Recortar el camino según los PM disponibles
        int maxSteps = m_remainingPM;
        if (static_cast<int>(path.size()) > maxSteps) {
            path.resize(maxSteps);
            std::cout << "Camino recortado a: " << path.size() << " pasos" << std::endl;
        }
        
        if (!path.empty()) {
            m_movementPath = path;
            m_movementTimer = 0.0f;
            m_isMovingToTarget = true;
            m_state = EntityState::Moving;
            std::cout << "Iniciando movimiento con " << path.size() << " pasos" << std::endl;
        }
    }
    std::cout << "=== FIN MOVIMIENTO ===" << std::endl;
}

void Entity::setPosition(sf::Vector2i position) {
    m_currentPosition = position;
    m_movementPath.clear();
    m_isMovingToTarget = false;
    m_state = EntityState::Idle;
}

std::vector<sf::Vector2i> Entity::getReachableTiles(const Map& map) const {
    return Pathfinding::getReachableTiles(map, m_currentPosition, m_remainingPM);
}

void Entity::startTurn() {
    m_remainingPM = m_totalPM;
    m_remainingPA = m_totalPA;
    m_state = EntityState::Idle;
}

void Entity::endTurn() {
    m_state = EntityState::Idle;
}

bool Entity::tryCastStrike(sf::Vector2i targetCell, Entity& target) {
    std::cout << "tryCastStrike: targetCell=(" << targetCell.x << "," << targetCell.y << "), targetPos=(" << target.getPosition().x << "," << target.getPosition().y << ")" << std::endl;
    
    // Verificar PA suficiente
    if (m_remainingPA < 3) {
        std::cout << "No hay PA suficientes: " << m_remainingPA << std::endl;
        return false;
    }
    
    // Verificar que el objetivo esté en la celda objetivo
    if (target.getPosition() != targetCell) {
        std::cout << "El objetivo no está en la celda objetivo" << std::endl;
        return false;
    }
    
    // Verificar alcance y LoS usando el nuevo sistema
    // Necesitamos acceso al mapa, lo pasaremos como parámetro
    // Por ahora mantenemos la verificación original
    if (!isInRange(targetCell, 3)) {
        std::cout << "Fuera de rango" << std::endl;
        return false;
    }
    
    // Ejecutar el golpe
    std::cout << "Golpe exitoso!" << std::endl;
    target.takeDamage(20);
    consumePA(3);
    
    return true;
}

void Entity::takeDamage(int damage) {
    m_hp = std::max(0, m_hp - damage);
}

bool Entity::isInRange(sf::Vector2i target, int maxRange) const {
    // Usar la misma lógica que LineOfSight para consistencia
    int dx = std::abs(target.x - m_currentPosition.x);
    int dy = std::abs(target.y - m_currentPosition.y);
    int distance = dx + dy; // Distancia Manhattan
    
    return distance <= maxRange;
}

void Entity::updateMovement(float deltaTime) {
    if (m_movementPath.empty()) {
        m_isMovingToTarget = false;
        m_state = EntityState::Idle;
        return;
    }
    
    m_movementTimer += deltaTime;
    
    if (m_movementTimer >= MOVEMENT_SPEED) {
        m_movementTimer = 0.0f;
        
        if (!m_movementPath.empty()) {
            m_currentPosition = m_movementPath.front();
            m_movementPath.erase(m_movementPath.begin());
            std::cout << "Paso completado. PM antes: " << m_remainingPM;
            consumePM(1); // Descontar 1 PM por cada paso
            std::cout << ", PM después: " << m_remainingPM << std::endl;
        }
    }
}

void Entity::updateScreenPosition(const Map& map) {
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

void Entity::consumePM(int amount) {
    m_remainingPM = std::max(0, m_remainingPM - amount);
}

void Entity::consumePA(int amount) {
    m_remainingPA = std::max(0, m_remainingPA - amount);
}

bool Entity::canCastSpell(sf::Vector2i targetCell, int minRange, int maxRange, const Map& map) const {
    std::cout << "canCastSpell: PA=" << m_remainingPA << ", target=(" << targetCell.x << "," << targetCell.y << ")" << std::endl;
    
    // Verificar PA suficiente
    if (m_remainingPA < 3) {
        std::cout << "No hay PA suficientes para castear" << std::endl;
        return false;
    }
    
    // Verificar rango
    if (!LineOfSight::isInRange(m_currentPosition, targetCell, minRange, maxRange)) {
        std::cout << "Fuera de rango para castear" << std::endl;
        return false;
    }
    
    // Verificar LoS
    if (!LineOfSight::hasLineOfSight(map, m_currentPosition, targetCell)) {
        std::cout << "Sin línea de visión para castear" << std::endl;
        return false;
    }
    
    std::cout << "Puede castear!" << std::endl;
    return true;
}

std::vector<sf::Vector2i> Entity::getCastableCells(const Map& map, int minRange, int maxRange) const {
    return LineOfSight::computeCastableCells(map, m_currentPosition, minRange, maxRange, true);
}

void Entity::castSpell(sf::Vector2i targetCell, int minRange, int maxRange, const Map& map, int paCost) {
    if (canCastSpell(targetCell, minRange, maxRange, map)) {
        consumePA(paCost);
        std::cout << "Hechizo lanzado!" << std::endl;
    }
}
