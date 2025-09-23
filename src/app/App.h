#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "map/Map.h"
#include "units/Entity.h"
#include "systems/TurnSystem.h"
#include "systems/Pathfinding.h"
#include "systems/LineOfSight.h"
#include "systems/Spells.h"
#include "systems/HUD.h"
#include "systems/Json.hpp"

class App {
public:
    App();
    void run();
    
private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    
    sf::RenderWindow m_window;
    Map m_map;
    TurnSystem m_turnSystem;
    Entity m_player;
    Entity m_enemy;
    std::vector<sf::Vector2i> m_reachableTiles;
    sf::Clock m_clock;
    
    // Sistema de targeting
    bool m_isTargeting;
    sf::Vector2i m_currentTargetCell;
    std::vector<sf::Vector2i> m_castableCells;
    
    // Sistema de hechizos
    int m_activeSpellIndex;
    const Spell* m_activeSpell;
    
    // Sistema de HUD
    HUD m_hud;
    
    // Sistema de mapas
    std::string m_currentMapFile;
    
    // Debug overlay
    bool gDebugOverlay = false;
    
    void updateReachableTiles();
    void renderReachableTiles();
    void updateWindowTitle();
    void handlePlayerInput(sf::Vector2f mousePos, sf::Mouse::Button button);
    
    // Sistema de targeting
    void enterTargetingMode();
    void exitTargetingMode();
    void updateTargeting(sf::Vector2f mousePos);
    void renderTargeting();
    void tryCastSpell(sf::Vector2i targetCell);
    
    // Sistema de hechizos
    void selectSpell(int spellIndex);
    void updateSpellTargeting();
    void renderSpellTargeting();
    
    // Sistema de HUD
    void updateHUD();
    
    // Sistema de carga/guardado de mapas
    void loadMapFromFile(const std::string& path);
    void saveMapToFile(const std::string& path);
    void reloadMap();
    
    // Sistema responsive
    float calculateVirtualScale() const;
};
