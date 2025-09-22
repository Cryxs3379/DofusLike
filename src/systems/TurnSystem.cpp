#include "systems/TurnSystem.h"
#include "systems/LineOfSight.h"
#include <algorithm>
#include <iostream>

TurnSystem::TurnSystem() : m_currentTurn(TurnState::Player), m_currentEntityIndex(0) {
}

void TurnSystem::addEntity(Entity* entity) {
    m_entities.push_back(entity);
}

void TurnSystem::startGame() {
    if (!m_entities.empty()) {
        m_currentEntityIndex = 0;
        m_currentTurn = TurnState::Player;
        m_entities[0]->startTurn();
    }
}

void TurnSystem::endCurrentTurn() {
    if (m_currentEntityIndex < m_entities.size()) {
        m_entities[m_currentEntityIndex]->endTurn();
    }
    nextTurn();
}

void TurnSystem::update(float deltaTime, const Map& map) {
    if (m_entities.empty()) return;
    
    // Actualizar la entidad actual
    if (m_currentEntityIndex < m_entities.size()) {
        m_entities[m_currentEntityIndex]->update(deltaTime);
        
        // Si es el turno del enemigo y no se está moviendo, ejecutar IA
        if (isEnemyTurn() && !m_entities[m_currentEntityIndex]->isMoving()) {
            executeEnemyAI(map);
        }
    }
}

TurnState TurnSystem::getCurrentTurn() const {
    return m_currentTurn;
}

Entity* TurnSystem::getCurrentEntity() const {
    if (m_currentEntityIndex < m_entities.size()) {
        return m_entities[m_currentEntityIndex];
    }
    return nullptr;
}

Entity* TurnSystem::getPlayer() const {
    for (auto* entity : m_entities) {
        if (entity->getType() == EntityType::Player) {
            return entity;
        }
    }
    return nullptr;
}

Entity* TurnSystem::getEnemy() const {
    for (auto* entity : m_entities) {
        if (entity->getType() == EntityType::Enemy) {
            return entity;
        }
    }
    return nullptr;
}

bool TurnSystem::isPlayerTurn() const {
    return m_currentTurn == TurnState::Player;
}

bool TurnSystem::isEnemyTurn() const {
    return m_currentTurn == TurnState::Enemy;
}

void TurnSystem::nextTurn() {
    m_currentEntityIndex = (m_currentEntityIndex + 1) % m_entities.size();
    m_currentTurn = (m_currentTurn == TurnState::Player) ? TurnState::Enemy : TurnState::Player;
    
    if (m_currentEntityIndex < m_entities.size()) {
        m_entities[m_currentEntityIndex]->startTurn();
    }
}

void TurnSystem::executeEnemyAI(const Map& map) {
    Entity* enemy = getCurrentEntity();
    Entity* player = getPlayer();
    
    if (!enemy || !player || !enemy->isAlive() || !player->isAlive()) {
        endCurrentTurn();
        return;
    }
    
    // Verificar si puede atacar usando el nuevo sistema de LoS
    if (enemy->getRemainingPA() >= 3 && enemy->canCastSpell(player->getPosition(), 1, 3, map)) {
        // Intentar atacar
        if (enemy->tryCastStrike(player->getPosition(), *player)) {
            std::cout << "Enemy ataca al Player!" << std::endl;
            endCurrentTurn();
            return;
        }
    }
    
    // Si no puede atacar, moverse hacia el player
    if (enemy->getRemainingPM() > 0) {
        std::vector<sf::Vector2i> reachableTiles = enemy->getReachableTiles(map);
        
        // Encontrar la casilla más cercana al player
        sf::Vector2i bestTile = enemy->getPosition();
        int bestDistance = std::abs(player->getPosition().x - enemy->getPosition().x) + 
                          std::abs(player->getPosition().y - enemy->getPosition().y);
        
        for (const auto& tile : reachableTiles) {
            int distance = std::abs(player->getPosition().x - tile.x) + 
                          std::abs(player->getPosition().y - tile.y);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestTile = tile;
            }
        }
        
        if (bestTile != enemy->getPosition()) {
            enemy->moveTo(bestTile, map);
        } else {
            // No puede moverse más, terminar turno
            endCurrentTurn();
        }
    } else {
        // Sin PM, terminar turno
        endCurrentTurn();
    }
}
