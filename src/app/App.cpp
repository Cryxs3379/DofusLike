#include "app/App.h"
#include <iostream>

App::App() : m_window(sf::VideoMode({1200u, 800u}), "DofusLike - Sistema de Turnos"),
             m_player(sf::Vector2i(7, 7), EntityType::Player),
             m_enemy(sf::Vector2i(10, 10), EntityType::Enemy),
             m_isTargeting(false),
             m_currentTargetCell(-1, -1) {
    
    // Configurar sistema de turnos
    m_turnSystem.addEntity(&m_player);
    m_turnSystem.addEntity(&m_enemy);
    m_turnSystem.startGame();
    
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
                // Tecla 1: activar/desactivar modo targeting
                if (m_turnSystem.isPlayerTurn() && !m_player.isMoving()) {
                    if (m_isTargeting) {
                        exitTargetingMode();
                    } else {
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
    
    // Recalcular casillas alcanzables si cambió el turno o la entidad se movió
    if (m_turnSystem.isPlayerTurn()) {
        updateReachableTiles();
        updateWindowTitle();
    }
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
    if (m_turnSystem.isPlayerTurn() && !m_player.isMoving()) {
        m_isTargeting = true;
        m_castableCells = m_player.getCastableCells(m_map, 1, 3);
        std::cout << "Modo targeting activado. Celdas válidas: " << m_castableCells.size() << std::endl;
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
    // Renderizar celdas casteables
    for (const auto& cell : m_castableCells) {
        sf::Vector2f screenPos = Isometric::isoToScreen(sf::Vector2i(cell.x, cell.y), sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE));
        screenPos += sf::Vector2f(400.0f, 300.0f); // Offset del mapa
        
        auto diamond = Isometric::createDiamond(sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE), sf::Color(255, 255, 0, 150));
        diamond.setPosition(screenPos);
        m_window.draw(diamond);
    }
    
    // Renderizar celda objetivo actual si es válida
    if (m_currentTargetCell.x >= 0 && m_currentTargetCell.y >= 0) {
        bool isValidTarget = std::find(m_castableCells.begin(), m_castableCells.end(), m_currentTargetCell) != m_castableCells.end();
        
        if (isValidTarget) {
            sf::Vector2f screenPos = Isometric::isoToScreen(sf::Vector2i(m_currentTargetCell.x, m_currentTargetCell.y), sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE));
            screenPos += sf::Vector2f(400.0f, 300.0f); // Offset del mapa
            
            auto diamond = Isometric::createDiamond(sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE), sf::Color(255, 0, 0, 200));
            diamond.setPosition(screenPos);
            m_window.draw(diamond);
        }
    }
}

void App::tryCastSpell(sf::Vector2i targetCell) {
    if (!m_isTargeting || !m_turnSystem.isPlayerTurn()) {
        std::cout << "No se puede castear: targeting=" << m_isTargeting << ", playerTurn=" << m_turnSystem.isPlayerTurn() << std::endl;
        return;
    }
    
    std::cout << "=== INTENTANDO CASTEAR ===" << std::endl;
    std::cout << "Target cell: (" << targetCell.x << "," << targetCell.y << ")" << std::endl;
    std::cout << "Enemy position: (" << m_enemy.getPosition().x << "," << m_enemy.getPosition().y << ")" << std::endl;
    std::cout << "Player PA: " << m_player.getRemainingPA() << std::endl;
    
    // Verificar si la celda es válida para castear
    bool isValidTarget = std::find(m_castableCells.begin(), m_castableCells.end(), targetCell) != m_castableCells.end();
    std::cout << "isValidTarget (en lista): " << isValidTarget << std::endl;
    
    // Verificar si puede castear (incluye PA, rango y LoS)
    bool canCast = m_player.canCastSpell(targetCell, 1, 3, m_map);
    std::cout << "canCast (verificación completa): " << canCast << std::endl;
    
    if (canCast) {
        // Verificar si hay un enemigo en la celda objetivo
        if (m_enemy.getPosition() == targetCell) {
            std::cout << "*** ATACANDO AL ENEMIGO ***" << std::endl;
            // Simplificar: atacar directamente sin verificación adicional
            m_enemy.takeDamage(20);
            m_player.consumePA(3);
            std::cout << "Golpe exitoso! Enemy HP: " << m_enemy.getHP() << std::endl;
        } else {
            // Castear sin objetivo (solo consume PA)
            std::cout << "*** CASTEANDO EN CELDA VACÍA ***" << std::endl;
            m_player.castSpell(targetCell, 1, 3, m_map, 3);
        }
        
        updateWindowTitle();
        
        // Salir del modo targeting si no hay PA suficientes
        if (m_player.getRemainingPA() < 3) {
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
