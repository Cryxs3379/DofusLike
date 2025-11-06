#pragma once
#include "units/Entity.h"
#include "map/Map.h"
#include <vector>

enum class TurnState {
    Player,
    Enemy
};

class TurnSystem {
public:
    TurnSystem();
    
    void addEntity(Entity* entity);
    void startGame();
    void endCurrentTurn();
    void update(float deltaTime, const Map& map);
    
    TurnState getCurrentTurn() const;
    Entity* getCurrentEntity() const;
    Entity* getPlayer() const;
    Entity* getEnemy() const;
    
    bool isPlayerTurn() const;
    bool isEnemyTurn() const;
    
private:
    std::vector<Entity*> m_entities;
    TurnState m_currentTurn;
    int m_currentEntityIndex;
    
    void nextTurn();
    void executeEnemyAI(const Map& map);
};
