#include "app/App.h"
#include <iostream>

App::App() : m_window(sf::VideoMode({1200u, 800u}), "DofusLike - Sistema de Turnos"),
             m_player(sf::Vector2i(7, 7), EntityType::Player),
             m_enemy(sf::Vector2i(10, 10), EntityType::Enemy),
             m_isTargeting(false),
             m_currentTargetCell(-1, -1),
             m_activeSpellIndex(0),
             m_activeSpell(nullptr),
             m_currentMapFile("data/map01.json") {
    
    // Configurar sistema de turnos
    m_turnSystem.addEntity(&m_player);
    m_turnSystem.addEntity(&m_enemy);
    m_turnSystem.startGame();
    
    // Inicializar hechizo activo
    selectSpell(0); // Empezar con "Golpe"
    
    // Configurar HUD
    m_hud.setWindowSize(m_window.getSize());
    
    // Cargar mapa inicial
    loadMapFromFile(m_currentMapFile);
    
    updateReachableTiles();
    updateWindowTitle();
}

void App::run() {
    while (m_window.isOpen()) {
        float deltaTime = m_clock.restart().asSeconds();
        
        handleEvents();
        update(deltaTime);
        render();
    }
}

void App::handleEvents() {
    while (auto ev = m_window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            m_window.close();
        }
        
        if (auto kb = ev->getIf<sf::Event::KeyPressed>()) {
            if (kb->code == sf::Keyboard::Key::Enter) {
                // Tecla Enter: finalizar turno del jugador
                if (m_turnSystem.isPlayerTurn() && !m_isTargeting) {
                    m_turnSystem.endCurrentTurn();
                    updateReachableTiles();
                    updateWindowTitle();
                }
            }
            else if (kb->code == sf::Keyboard::Key::Space) {
                // Tecla Espacio: castear hechizo en modo targeting
                if (m_turnSystem.isPlayerTurn() && m_isTargeting) {
                    tryCastSpell(m_currentTargetCell);
                }
            }
            else if (kb->code == sf::Keyboard::Key::Num1) {
                // Tecla 1: seleccionar hechizo "Golpe" y activar targeting
                if (m_turnSystem.isPlayerTurn() && !m_player.isMoving()) {
                    selectSpell(0);
                    if (!m_isTargeting) {
                        enterTargetingMode();
                    }
                }
            }
            else if (kb->code == sf::Keyboard::Key::Num2) {
                // Tecla 2: seleccionar hechizo "Flecha" y activar targeting
                if (m_turnSystem.isPlayerTurn() && !m_player.isMoving()) {
                    selectSpell(1);
                    if (!m_isTargeting) {
                        enterTargetingMode();
                    }
                }
            }
            else if (kb->code == sf::Keyboard::Key::Num3) {
                // Tecla 3: seleccionar hechizo "Curar" y activar targeting
                if (m_turnSystem.isPlayerTurn() && !m_player.isMoving()) {
                    selectSpell(2);
                    if (!m_isTargeting) {
                        enterTargetingMode();
                    }
                }
            }
            else if (kb->code == sf::Keyboard::Key::Escape) {
                // Tecla Esc: cancelar modo targeting
                if (m_isTargeting) {
                    exitTargetingMode();
                }
            }
            else if (kb->code == sf::Keyboard::Key::F5) {
                // Tecla F5: recargar mapa
                reloadMap();
            }
            else if (kb->code == sf::Keyboard::Key::F6) {
                // Tecla F6: guardar mapa actual
                saveMapToFile("data/map01.saved.json");
            }
            else if (kb->code == sf::Keyboard::Key::F8) {
                // Tecla F8: toggle debug overlay
                gDebugOverlay = !gDebugOverlay;
                std::cout << "Debug overlay: " << (gDebugOverlay ? "ON" : "OFF") << std::endl;
            }
        }
        
        if (auto mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
            sf::Vector2f mousePos = sf::Vector2f(mb->position.x, mb->position.y);
            handlePlayerInput(mousePos, mb->button);
        }
        
        if (auto mm = ev->getIf<sf::Event::MouseMoved>()) {
            sf::Vector2f mousePos = sf::Vector2f(mm->position.x, mm->position.y);
            m_map.updateHover(mousePos);
            
            // Actualizar targeting si está activo
            if (m_isTargeting) {
                updateTargeting(mousePos);
            }
        }
    }
}

void App::update(float deltaTime) {
    m_turnSystem.update(deltaTime, m_map);
    
    // Recalcular casillas alcanzables solo cuando sea necesario
    static int lastPlayerPM = -1;
    static sf::Vector2i lastPlayerPos(-1, -1);
    
    if (m_turnSystem.isPlayerTurn()) {
        // Solo recalcular si cambió la posición O los PM
        int currentPM = m_player.getRemainingPM();
        sf::Vector2i currentPos = m_player.getPosition();
        
        if (currentPM != lastPlayerPM || currentPos != lastPlayerPos) {
            std::cout << "=== CAMBIO DETECTADO ===" << std::endl;
            std::cout << "PM cambió: " << lastPlayerPM << " -> " << currentPM << std::endl;
            std::cout << "Pos cambió: (" << lastPlayerPos.x << "," << lastPlayerPos.y << ") -> (" << currentPos.x << "," << currentPos.y << ")" << std::endl;
            updateReachableTiles();
            lastPlayerPM = currentPM;
            lastPlayerPos = currentPos;
            std::cout << "=== FIN CAMBIO ===" << std::endl;
        }
        updateWindowTitle();
    }
    
    // Actualizar HUD
    updateHUD();
}

void App::render() {
    m_window.clear(sf::Color(50, 50, 50)); // Fondo gris oscuro
    
    // Renderizar el mapa
    m_map.render(m_window);
    
    // Renderizar casillas alcanzables solo en turno del jugador
    if (m_turnSystem.isPlayerTurn() && !m_isTargeting) {
        renderReachableTiles();
    }
    
    // Renderizar targeting si está activo
    if (m_isTargeting) {
        renderTargeting();
    }
    
    // Renderizar entidades
    m_player.render(m_window, m_map);
    m_enemy.render(m_window, m_map);
    
    // Renderizar HUD (siempre al final, encima de todo)
    m_hud.draw(m_window);
    
    // Debug overlay
    if (gDebugOverlay) {
        // Dibujar '+' en la posición de cada unidad
        sf::RectangleShape crossH(sf::Vector2f(10, 2));
        sf::RectangleShape crossV(sf::Vector2f(2, 10));
        crossH.setFillColor(sf::Color::Yellow);
        crossV.setFillColor(sf::Color::Yellow);
        
        // Player
        sf::Vector2f playerPos = m_map.getTileCenter(m_player.getPosition().x, m_player.getPosition().y);
        crossH.setPosition({playerPos.x - 5, playerPos.y - 1});
        crossV.setPosition({playerPos.x - 1, playerPos.y - 5});
        m_window.draw(crossH);
        m_window.draw(crossV);
        
        // Enemy
        sf::Vector2f enemyPos = m_map.getTileCenter(m_enemy.getPosition().x, m_enemy.getPosition().y);
        crossH.setPosition({enemyPos.x - 5, enemyPos.y - 1});
        crossV.setPosition({enemyPos.x - 1, enemyPos.y - 5});
        m_window.draw(crossH);
        m_window.draw(crossV);
        
        // Dibujar bounds del sprite del player
        sf::FloatRect bounds = m_player.getGlobalBounds();
        sf::RectangleShape boundsRect(sf::Vector2f(bounds.size.x, bounds.size.y));
        boundsRect.setPosition({bounds.position.x, bounds.position.y});
        boundsRect.setFillColor(sf::Color::Transparent);
        boundsRect.setOutlineColor(sf::Color::Red);
        boundsRect.setOutlineThickness(2);
        m_window.draw(boundsRect);
    }
    
    m_window.display();
}

void App::updateReachableTiles() {
    if (m_turnSystem.isPlayerTurn()) {
        std::cout << "=== RECALCULANDO CELDAS ALCANZABLES ===" << std::endl;
        std::cout << "Posición del jugador: (" << m_player.getPosition().x << "," << m_player.getPosition().y << ")" << std::endl;
        std::cout << "PM restantes: " << m_player.getRemainingPM() << std::endl;
        m_reachableTiles = m_player.getReachableTiles(m_map);
        std::cout << "Celdas alcanzables: " << m_reachableTiles.size() << std::endl;
        std::cout << "=== FIN RECÁLCULO ===" << std::endl;
    }
}

void App::renderReachableTiles() {
    for (const auto& tile : m_reachableTiles) {
        sf::Vector2f screenPos = Isometric::isoToScreen(sf::Vector2i(tile.x, tile.y), sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE));
        screenPos += sf::Vector2f(400.0f, 300.0f); // Offset del mapa
        
        auto diamond = Isometric::createDiamond(sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE), sf::Color(0, 255, 255, 100));
        diamond.setPosition(screenPos);
        m_window.draw(diamond);
    }
}

void App::updateWindowTitle() {
    std::string turnStr = m_turnSystem.isPlayerTurn() ? "Player" : "Enemy";
    std::string title = "Turno: " + turnStr + 
                       " | PA: " + std::to_string(m_player.getRemainingPA()) + 
                       " | PM: " + std::to_string(m_player.getRemainingPM()) + 
                       " | Player HP: " + std::to_string(m_player.getHP()) + 
                       " | Enemy HP: " + std::to_string(m_enemy.getHP());
    
    // Agregar información del hechizo activo si es el turno del jugador
    if (m_turnSystem.isPlayerTurn() && m_activeSpell) {
        title += " | Hechizo: " + m_activeSpell->name + " (PA:" + std::to_string(m_activeSpell->costPA) + ")";
    }
    
    m_window.setTitle(title);
}

void App::handlePlayerInput(sf::Vector2f mousePos, sf::Mouse::Button button) {
    if (!m_turnSystem.isPlayerTurn()) return;
    
    if (button == sf::Mouse::Button::Right) {
        // Clic derecho: alternar loseta
        m_map.handleMouseClick(mousePos, sf::Mouse::Button::Right);
        updateReachableTiles();
    }
    else if (button == sf::Mouse::Button::Left) {
        if (m_isTargeting) {
            // Clic izquierdo en modo targeting: castear hechizo
            tryCastSpell(m_currentTargetCell);
        } else {
            // Clic izquierdo: mover jugador si la casilla es alcanzable y no está moviéndose
            if (!m_player.isMoving()) {
                sf::Vector2i targetTile = m_map.getTileFromPosition(mousePos);
                
                // Verificar si la casilla es alcanzable
                bool isReachable = std::find(m_reachableTiles.begin(), m_reachableTiles.end(), targetTile) != m_reachableTiles.end();
                
                if (isReachable && targetTile != m_player.getPosition()) {
                    m_player.moveTo(targetTile, m_map);
                }
            }
        }
    }
}

void App::enterTargetingMode() {
    if (m_turnSystem.isPlayerTurn() && !m_player.isMoving() && m_activeSpell) {
        m_isTargeting = true;
        m_castableCells = m_player.getCastableCells(*m_activeSpell, m_map);
        std::cout << "Modo targeting activado para " << m_activeSpell->name << ". Celdas válidas: " << m_castableCells.size() << std::endl;
    }
}

void App::exitTargetingMode() {
    m_isTargeting = false;
    m_currentTargetCell = sf::Vector2i(-1, -1);
    m_castableCells.clear();
    std::cout << "Modo targeting desactivado." << std::endl;
}

void App::updateTargeting(sf::Vector2f mousePos) {
    if (!m_isTargeting) return;
    
    sf::Vector2i targetCell = m_map.getTileFromPosition(mousePos);
    m_currentTargetCell = targetCell;
}

void App::renderTargeting() {
    if (!m_activeSpell) return;
    
    // Renderizar celdas casteables con el color del hechizo
    sf::Color spellColor = m_activeSpell->color;
    spellColor.a = 150; // Hacer semi-transparente
    
    for (const auto& cell : m_castableCells) {
        sf::Vector2f screenPos = Isometric::isoToScreen(sf::Vector2i(cell.x, cell.y), sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE));
        screenPos += sf::Vector2f(400.0f, 300.0f); // Offset del mapa
        
        auto diamond = Isometric::createDiamond(sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE), spellColor);
        diamond.setPosition(screenPos);
        m_window.draw(diamond);
    }
    
    // Renderizar celda objetivo actual si es válida
    if (m_currentTargetCell.x >= 0 && m_currentTargetCell.y >= 0) {
        bool isValidTarget = std::find(m_castableCells.begin(), m_castableCells.end(), m_currentTargetCell) != m_castableCells.end();
        
        if (isValidTarget) {
            sf::Vector2f screenPos = Isometric::isoToScreen(sf::Vector2i(m_currentTargetCell.x, m_currentTargetCell.y), sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE));
            screenPos += sf::Vector2f(400.0f, 300.0f); // Offset del mapa
            
            // Color más brillante para la celda objetivo
            sf::Color targetColor = m_activeSpell->color;
            targetColor.a = 200;
            auto diamond = Isometric::createDiamond(sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE), targetColor);
            diamond.setPosition(screenPos);
            m_window.draw(diamond);
        }
    }
}

void App::tryCastSpell(sf::Vector2i targetCell) {
    if (!m_isTargeting || !m_turnSystem.isPlayerTurn() || !m_activeSpell) {
        std::cout << "No se puede castear: targeting=" << m_isTargeting << ", playerTurn=" << m_turnSystem.isPlayerTurn() << ", activeSpell=" << (m_activeSpell ? m_activeSpell->name : "null") << std::endl;
        return;
    }
    
    std::cout << "=== INTENTANDO CASTEAR " << m_activeSpell->name << " ===" << std::endl;
    std::cout << "Target cell: (" << targetCell.x << "," << targetCell.y << ")" << std::endl;
    std::cout << "Enemy position: (" << m_enemy.getPosition().x << "," << m_enemy.getPosition().y << ")" << std::endl;
    std::cout << "Player PA: " << m_player.getRemainingPA() << std::endl;
    
    // Verificar si la celda es válida para castear
    bool isValidTarget = std::find(m_castableCells.begin(), m_castableCells.end(), targetCell) != m_castableCells.end();
    std::cout << "isValidTarget (en lista): " << isValidTarget << std::endl;
    
    // Verificar si puede castear (incluye PA, rango y LoS)
    bool canCast = m_player.canCastSpell(*m_activeSpell, targetCell, m_map);
    std::cout << "canCast (verificación completa): " << canCast << std::endl;
    
    if (canCast) {
        // Verificar si hay un enemigo en la celda objetivo
        if (m_enemy.getPosition() == targetCell) {
            std::cout << "*** LANZANDO " << m_activeSpell->name << " AL ENEMIGO ***" << std::endl;
            bool success = m_player.castSpell(*m_activeSpell, targetCell, m_map, m_enemy);
            if (success) {
                std::cout << "Hechizo lanzado exitosamente!" << std::endl;
            }
        } else {
            std::cout << "*** NO HAY OBJETIVO EN LA CELDA ***" << std::endl;
            std::cout << "No se puede lanzar " << m_activeSpell->name << " sin objetivo" << std::endl;
        }
        
        updateWindowTitle();
        
        // Salir del modo targeting si no hay PA suficientes para otro hechizo
        if (m_player.getRemainingPA() < m_activeSpell->costPA) {
            exitTargetingMode();
        }
    } else {
        std::cout << "*** NO SE PUEDE CASTEAR ***" << std::endl;
        if (!isValidTarget) {
            std::cout << "Razón: Celda no está en la lista de válidas" << std::endl;
        }
    }
    std::cout << "=== FIN INTENTO CASTEO ===" << std::endl;
}

// Métodos del sistema de hechizos
void App::selectSpell(int spellIndex) {
    const auto& spells = Spells::getPlayerSpells();
    if (spellIndex >= 0 && spellIndex < static_cast<int>(spells.size())) {
        m_activeSpellIndex = spellIndex;
        m_activeSpell = &spells[spellIndex];
        std::cout << "Hechizo seleccionado: " << m_activeSpell->name << " (PA: " << m_activeSpell->costPA << ")" << std::endl;
        
        // Si estamos en modo targeting, actualizar las celdas casteables
        if (m_isTargeting) {
            updateSpellTargeting();
        }
    } else {
        std::cout << "Índice de hechizo inválido: " << spellIndex << std::endl;
    }
}

void App::updateSpellTargeting() {
    if (m_activeSpell && m_turnSystem.isPlayerTurn()) {
        m_castableCells = m_player.getCastableCells(*m_activeSpell, m_map);
        std::cout << "Celdas casteables actualizadas para " << m_activeSpell->name << ": " << m_castableCells.size() << std::endl;
    }
}

void App::renderSpellTargeting() {
    // Este método se llama desde render() cuando estamos en modo targeting
    renderTargeting();
}

void App::updateHUD() {
    // Actualizar estadísticas del jugador
    m_hud.setPlayerStats(
        m_player.getHP(),
        100, // maxHP (hardcoded por ahora)
        m_player.getRemainingPA(),
        m_player.getTotalPA(),
        m_player.getRemainingPM(),
        m_player.getTotalPM()
    );
    
    // Actualizar estadísticas del enemigo
    m_hud.setEnemyStats(
        m_enemy.getHP(),
        100, // maxHP (hardcoded por ahora)
        m_enemy.getRemainingPA(),
        m_enemy.getTotalPA(),
        m_enemy.getRemainingPM(),
        m_enemy.getTotalPM()
    );
    
    // Actualizar turno actual
    m_hud.setTurn(m_turnSystem.isPlayerTurn());
    
    // Actualizar hechizo activo
    m_hud.setActiveSpellIndex(m_activeSpellIndex);
    
    // Actualizar tamaño de ventana si cambió
    m_hud.setWindowSize(m_window.getSize());
}

void App::loadMapFromFile(const std::string& path) {
    MapData mapData;
    if (JsonParser::loadMapFromFile(path, mapData)) {
        if (m_map.loadFromArray(mapData.width, mapData.height, mapData.blocked)) {
            std::cout << "Mapa cargado exitosamente desde: " << path << std::endl;
            m_currentMapFile = path;
            
            // Recalcular todo después de cargar el mapa
            updateReachableTiles();
            if (m_isTargeting) {
                updateSpellTargeting();
            }
        } else {
            std::cout << "Error: No se pudo cargar el mapa en la estructura interna" << std::endl;
        }
    } else {
        std::cout << "Error: No se pudo cargar el archivo de mapa: " << path << std::endl;
        std::cout << "Usando mapa por defecto (15x15 sin bloqueos)" << std::endl;
    }
}

void App::saveMapToFile(const std::string& path) {
    MapData mapData;
    mapData.width = m_map.getWidth();
    mapData.height = m_map.getHeight();
    mapData.blocked = m_map.exportBlockedLinear();
    mapData.valid = true;
    
    if (JsonParser::saveMapToFile(path, mapData)) {
        std::cout << "Mapa guardado exitosamente en: " << path << std::endl;
    } else {
        std::cout << "Error: No se pudo guardar el mapa en: " << path << std::endl;
    }
}

void App::reloadMap() {
    std::cout << "=== RECARGANDO MAPA ===" << std::endl;
    loadMapFromFile(m_currentMapFile);
    std::cout << "=== FIN RECARGA ===" << std::endl;
}
