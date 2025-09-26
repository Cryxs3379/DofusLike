#include "units/Entity.h"
#include "systems/LineOfSight.h"
#include "systems/Spells.h"
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
      m_state(EntityState::Idle),
      m_currentDirection(0),
      m_sprite(*Assets::getEmptyTexture()) {
    
    // Configurar círculo de fallback
    m_entityShape.setRadius(8.0f);
    m_entityShape.setFillColor(type == EntityType::Player ? sf::Color::Blue : sf::Color::Red);
    m_entityShape.setOutlineColor(sf::Color::White);
    m_entityShape.setOutlineThickness(2.0f);
    m_entityShape.setOrigin({8.0f, 8.0f});
    
    // Cargar el mismo spritesheet para player y enemy para consistencia de tamaño
    std::cout << "[Entity] Loading sprites for " << (type == EntityType::Player ? "player" : "enemy") << "..." << std::endl;
    m_texture = Assets::getTexture("assets/sprites/player.png");
    
    if (m_texture) {
        // Usar el mismo sprite para todas las direcciones (el spritesheet tiene todas las animaciones)
        for (int i = 0; i < 5; i++) {
            m_textures[i] = m_texture;
        }
        std::cout << "[Entity] Main texture: " << (m_texture ? "LOADED" : "FAILED") << std::endl;
    } else {
        // Fallback a sprites individuales si no se encuentra el spritesheet
        if (type == EntityType::Player) {
            m_textures[0] = Assets::getTexture("assets/sprites/player_idle.png");      // Idle
            m_textures[1] = Assets::getTexture("assets/sprites/player_right.png");    // Derecha
            m_textures[2] = Assets::getTexture("assets/sprites/player_left.png");     // Izquierda
            m_textures[3] = Assets::getTexture("assets/sprites/player_forward.png");  // Adelante
            m_textures[4] = Assets::getTexture("assets/sprites/player_back.png");     // Atrás
        } else {
            // Para enemigos, usar los mismos sprites individuales que el player
            m_textures[0] = Assets::getTexture("assets/sprites/player_idle.png");
            m_textures[1] = Assets::getTexture("assets/sprites/player_right.png");    // Derecha
            m_textures[2] = Assets::getTexture("assets/sprites/player_left.png");     // Izquierda
            m_textures[3] = Assets::getTexture("assets/sprites/player_forward.png");  // Adelante
            m_textures[4] = Assets::getTexture("assets/sprites/player_back.png");     // Atr�s
        }
        
        for (int i = 0; i < 5; i++) {
            std::cout << "[Entity] Texture " << i << ": " << (m_textures[i] ? "LOADED" : "FAILED") << std::endl;
        }
    }
    
    // Cargar animaciones de combate (para ambos player y enemy)
    m_combatTextures[0] = Assets::getTexture("assets/sprites/ataqueespadaa.png"); // Ataque con espada
    m_combatTextures[1] = Assets::getTexture("assets/sprites/ataquearco.png");    // Ataque con arco
    m_combatTextures[2] = Assets::getTexture("assets/sprites/heal.png");          // Curación
    
    for (int i = 0; i < 3; i++) {
        std::cout << "[Entity] Combat Texture " << i << ": " << (m_combatTextures[i] ? "LOADED" : "FAILED") << std::endl;
    }
    
    // Verificar si al menos una textura se cargó
    bool hasTexture = false;
    for (int i = 0; i < 5; i++) {
        if (m_textures[i]) {
            hasTexture = true;
            break;
        }
    }
    
    if (hasTexture) {
        // Configurar sprite con la textura idle por defecto
        if (m_textures[0]) {
            m_sprite.setTexture(*m_textures[0]);
        } else {
            // Si no hay idle, usar la primera disponible
            for (int i = 1; i < 5; i++) {
                if (m_textures[i]) {
                    m_sprite.setTexture(*m_textures[i]);
                    break;
                }
            }
        }
        
        // Configurar animación para spritesheet (usar la misma lógica que Pawn)
        const sf::Texture& tex = m_sprite.getTexture();
        auto texSize = tex.getSize();
        std::cout << "[Entity] Texture size: " << texSize.x << "x" << texSize.y << std::endl;
        
        // Usar la misma lógica de detección que Pawn.cpp
        if (texSize.x == 720 && texSize.y == 330) {
            // Spritesheet del player: 8 columnas, 3 filas -> 90x110 por frame
            m_anim.columns = 8;
            m_anim.frameSize = {90u, 110u};
            std::cout << "[Entity] Detected player spritesheet (8x3 grid, 90x110 frames)" << std::endl;
        } else if (texSize.x == 1024 && texSize.y == 1024) {
            // Sprites de 1024x1024 organizados en grid 3x3 -> 341x341 por frame
            m_anim.columns = 3;
            m_anim.frameSize = {texSize.x / 3, texSize.y / 3}; // 341x341
            std::cout << "[Entity] Detected 3x3 grid (1024x1024 -> 341x341 frames)" << std::endl;
        } else if (texSize.x == 288 && texSize.y == 288) { // 3x3 grid de 96x96
            m_anim.columns = 3;
            m_anim.frameSize = {96u, 96u};
            std::cout << "[Entity] Detected 3x3 grid (96x96 frames)" << std::endl;
        } else if (texSize.x == 96 && texSize.y == 96) { // 1 frame
            m_anim.columns = 1;
            m_anim.frameSize = {0u, 0u}; // Desactivar animación
            std::cout << "[Entity] Detected single frame (96x96)" << std::endl;
        } else {
            // Fallback: no animar, usar textura completa
            m_anim.columns = 1;
            m_anim.frameSize = {0u, 0u}; // hará que Animation::apply no toque el rect
            std::cout << "[Entity] Using single frame mode for size " << texSize.x << "x" << texSize.y << std::endl;
        }
        
        m_anim.row = 0;
        m_anim.current = 0;
        m_anim.timer = 0.f;
        m_anim.frameDuration = 0.12f;
        
        // Configurar origin y escala (usar la misma lógica que Pawn.cpp)
        sf::Vector2i frameSize(static_cast<int>(m_anim.frameSize.x), static_cast<int>(m_anim.frameSize.y));
        if (frameSize.x == 0 || frameSize.y == 0) {
            // Fallback: usar tamaño completo de textura
            frameSize = sf::Vector2i(static_cast<int>(texSize.x), static_cast<int>(texSize.y));
        }
        
        // Origin en los pies (bottom-center) con el mismo padding que Pawn
        m_sprite.setOrigin({ frameSize.x * 0.5f, static_cast<float>(frameSize.y) - FOOT_PADDING });

        // Escala: ajustar según el tamaño de la textura
        const float targetHeight = Map::TILE_SIZE * kTileHeightMultiplier;
        float scale;
        
        if (texSize.x == 1024 && texSize.y == 1024) {
            // Para sprites de 1024x1024 con grid 3x3, usar el tamaño del frame
            scale = (targetHeight / static_cast<float>(frameSize.y)) * 0.9f; // Usar frameSize, no texSize
        } else {
            // Para otros tamaños, usar la fórmula original
            scale = (targetHeight / static_cast<float>(frameSize.y)) * 0.9f;
        }
        
        if (!std::isfinite(scale) || scale <= 0.f) scale = 1.f;
        m_sprite.setScale({scale, scale});

        // No usar offset adicional para centrar correctamente en la loseta
        m_spriteOffset = {-14.f, 0.f};
        m_useSprite = true;
        
        std::cout << "[Entity] sprite ON size=" << frameSize.x << "x" << frameSize.y
                  << " scale=" << scale << " anim=" << m_anim.columns << "x" << m_anim.frameSize.x << "x" << m_anim.frameSize.y << std::endl;
    } else {
        m_useSprite = false;
        std::cout << "[Entity] sprite OFF (fallback)" << std::endl;
    }
}

void Entity::update(float deltaTime) {
    updateMovement(deltaTime);
    
    // Manejar animaciones de combate
    if (m_currentCombatAnimation >= 0) {
        m_combatAnimationTimer += deltaTime;
        if (m_combatAnimationTimer >= COMBAT_ANIMATION_DURATION) {
            stopCombatAnimation();
        } else {
            // Actualizar animación de combate
            m_anim.update(deltaTime);
        }
    } else if (m_useSprite) {
        // Animación normal de movimiento
        if (m_state == EntityState::Moving) {
            m_anim.update(deltaTime);
        } else {
            m_anim.reset();
        }
    }
}

void Entity::render(sf::RenderWindow& window, const Map& map) {
    updateScreenPosition(map);
    
    if (m_useSprite) {
        // Aplicar el frame actual (la animación avanza en update cuando corresponde)
        m_anim.apply(m_sprite);
        
        const sf::Vector2f drawPos = m_screenPosition + m_spriteOffset; // ahora offset = {0,0}
        m_sprite.setPosition({std::round(drawPos.x), std::round(drawPos.y)});
        
        // Debug para animaciones de combate
        if (m_currentCombatAnimation >= 0) {
            static int debugCounter = 0;
            if (debugCounter % 60 == 0) { // Cada 60 frames (1 segundo a 60fps)
                std::cout << "[Entity] Combat animation " << m_currentCombatAnimation << " - Frame: " << m_anim.current << ", Row: " << m_anim.row << ", Timer: " << m_combatAnimationTimer << std::endl;
            }
            debugCounter++;
        }
        
        window.draw(m_sprite);
    } else {
        // Fallback al círculo
        m_entityShape.setPosition(m_screenPosition);
        window.draw(m_entityShape);
    }
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
        // Eliminar el primer nodo si es igual a la posición actual
        if (!path.empty() && path.front() == m_currentPosition) {
            path.erase(path.begin());
            std::cout << "Eliminado primer nodo (posición actual)" << std::endl;
        }
        
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
    
    // Iniciar animación de combate para el enemy
    startCombatAnimation(0); // ataqueespadaa.png
    
    target.takeDamage(20);
    consumePA(3);
    
    // Asegurar que el enemy vuelva al estado idle después del ataque
    setDirection(0); // Idle
    
    return true;
}

void Entity::takeDamage(int damage) {
    if (damage > 0) {
        // Daño normal
        m_hp = std::max(0, m_hp - damage);
    } else if (damage < 0) {
        // Curación (damage negativo)
        m_hp = std::min(100, m_hp - damage); // -(-15) = +15
    }
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
        if (m_useSprite) {
            setDirection(0); // Volver a idle
        }
        return;
    }
    
    // Determinar dirección de movimiento para animación (usar la misma lógica que Pawn)
    if (m_useSprite && !m_movementPath.empty()) {
        sf::Vector2i nextPos = m_movementPath.front();
        sf::Vector2i direction = nextPos - m_currentPosition;
        
        // Convertir dirección a índice de fila (igual que Pawn: 1=up, 2=left, 3=down, 4=right)
        int newDirection = 0; // idle
        if (direction.y < 0) newDirection = 1;      // Up
        else if (direction.x < 0) newDirection = 2; // Left
        else if (direction.y > 0) newDirection = 3; // Down
        else if (direction.x > 0) newDirection = 4; // Right
        
        // Cambiar dirección si cambió
        if (newDirection != m_currentDirection) {
            setDirection(newDirection);
        }
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
        
        if (m_movementPath.empty()) {
            m_isMovingToTarget = false;
            m_state = EntityState::Idle;
            if (m_useSprite) {
                setDirection(0); // Volver a idle
            }
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

sf::FloatRect Entity::getGlobalBounds() const {
    if (m_useSprite) {
        return m_sprite.getGlobalBounds();
    } else {
        return m_entityShape.getGlobalBounds();
    }
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

// Nuevos métodos del sistema de hechizos mejorado
bool Entity::canCastSpell(const Spell& spell, sf::Vector2i targetCell, const Map& map) const {
    std::cout << "canCastSpell: " << spell.name << " PA=" << m_remainingPA << ", target=(" << targetCell.x << "," << targetCell.y << ")" << std::endl;
    
    // Verificar PA suficiente
    if (m_remainingPA < spell.costPA) {
        std::cout << "No hay PA suficientes para " << spell.name << " (necesita " << spell.costPA << ", tiene " << m_remainingPA << ")" << std::endl;
        return false;
    }
    
    // Verificar rango
    if (!LineOfSight::isInRange(m_currentPosition, targetCell, spell.minRange, spell.maxRange)) {
        std::cout << "Fuera de rango para " << spell.name << " (min=" << spell.minRange << ", max=" << spell.maxRange << ")" << std::endl;
        return false;
    }
    
    // Verificar LoS si es necesario
    if (spell.needsLoS && !LineOfSight::hasLineOfSight(map, m_currentPosition, targetCell)) {
        std::cout << "Sin línea de visión para " << spell.name << std::endl;
        return false;
    }
    
    std::cout << "Puede castear " << spell.name << "!" << std::endl;
    return true;
}

std::vector<sf::Vector2i> Entity::getCastableCells(const Spell& spell, const Map& map) const {
    return LineOfSight::computeCastableCells(map, m_currentPosition, spell.minRange, spell.maxRange, spell.needsLoS);
}

bool Entity::castSpell(const Spell& spell, sf::Vector2i targetCell, const Map& map, Entity& target) {
    if (canCastSpell(spell, targetCell, map)) {
        // Verificar que el objetivo esté en la celda objetivo
        if (target.getPosition() == targetCell) {
            std::cout << "*** LANZANDO " << spell.name << " ***" << std::endl;
            applyEffect(spell, target);
            consumePA(spell.costPA);
            std::cout << "Hechizo " << spell.name << " lanzado exitosamente!" << std::endl;
            return true;
        } else {
            std::cout << "No hay objetivo en la celda (" << targetCell.x << "," << targetCell.y << ")" << std::endl;
        }
    }
    return false;
}

void Entity::applyEffect(const Spell& spell, Entity& target) {
    if (spell.effectType == EffectType::Damage) {
        target.takeDamage(spell.value);
        std::cout << spell.name << " inflige " << spell.value << " de daño. HP objetivo: " << target.getHP() << std::endl;
    } else if (spell.effectType == EffectType::Heal) {
        // Para curar, necesitamos un método público o hacer m_hp público
        // Por ahora, usaremos takeDamage con valor negativo
        target.takeDamage(-spell.value);
        std::cout << spell.name << " cura " << spell.value << " HP. HP objetivo: " << target.getHP() << std::endl;
    }
}

void Entity::setDirection(int direction) {
    if (direction < 0 || direction > 4) return;
    
    m_currentDirection = direction;
    
    // No cambiar textura si estamos en animación de combate
    if (m_currentCombatAnimation >= 0) {
        return;
    }
    
    if (m_useSprite) {
        // Si tenemos el spritesheet principal, usar animación por filas
        if (m_texture) {
            // Mapear dirección a fila del spritesheet 3x3
            // 0=idle, 1=up, 2=left, 3=down, 4=right
            int row = 0; // idle por defecto
            if (direction == 1) row = 0;      // Up (fila 0)
            else if (direction == 2) row = 1; // Left (fila 1) 
            else if (direction == 3) row = 0; // Down (fila 0)
            else if (direction == 4) row = 2; // Right (fila 2)
            
            m_anim.setDirection(row);
        } else {
            // Fallback: usar texturas separadas
            if (m_textures[direction]) {
                m_sprite.setTexture(*m_textures[direction]);
                m_anim.setDirection(0); // Resetear animación
            }
        }
    }
}

// Implementación de métodos de animaciones de combate
void Entity::startCombatAnimation(int animationType) {
    if (animationType >= 0 && animationType < 3 && m_combatTextures[animationType]) {
        m_currentCombatAnimation = animationType;
        m_combatAnimationTimer = 0.f;
        
        // Configurar el sprite para la animación de combate
        m_sprite.setTexture(*m_combatTextures[animationType]);
        
        // Configurar la animación para spritesheet 3x3
        m_anim.frameSize = sf::Vector2u(341, 341); // Tamaño de frame para 1024x1024 / 3
        m_anim.columns = 3;
        m_anim.row = 0; // Empezar en la primera fila
        m_anim.current = 0;
        m_anim.timer = 0.f;
        m_anim.frameDuration = 0.15f; // 150ms por frame (más rápido)
        
        // Asegurar que el sprite esté visible
        m_useSprite = true;
        
        // Aplicar inmediatamente el primer frame
        m_anim.apply(m_sprite);
        
        std::cout << "[Entity] Started combat animation: " << animationType << " (texture: " << (m_combatTextures[animationType] ? "OK" : "NULL") << ")" << std::endl;
    } else {
        std::cout << "[Entity] ERROR: Cannot start combat animation " << animationType << " (texture: " << (m_combatTextures[animationType] ? "OK" : "NULL") << ")" << std::endl;
    }
}

void Entity::stopCombatAnimation() {
    if (m_currentCombatAnimation >= 0) {
        std::cout << "[Entity] Stopped combat animation: " << m_currentCombatAnimation << std::endl;
        m_currentCombatAnimation = -1;
        m_combatAnimationTimer = 0.f;
        
        // Volver a la textura de movimiento normal
        // Asegurar que tenemos una dirección válida
        if (m_currentDirection < 0 || m_currentDirection > 4) {
            m_currentDirection = 0; // Idle por defecto
            std::cout << "[Entity] Reset direction to idle (0)" << std::endl;
        }
        
        if (m_textures[m_currentDirection]) {
            m_sprite.setTexture(*m_textures[m_currentDirection]);
            std::cout << "[Entity] Restored movement texture for direction: " << m_currentDirection << std::endl;
        } else {
            std::cout << "[Entity] ERROR: No movement texture found for direction: " << m_currentDirection << std::endl;
            // Fallback a la primera textura disponible
            for (int i = 0; i < 5; i++) {
                if (m_textures[i]) {
                    m_sprite.setTexture(*m_textures[i]);
                    m_currentDirection = i;
                    std::cout << "[Entity] Fallback to texture " << i << std::endl;
                    break;
                }
            }
        }
        
        // Resetear animación a movimiento normal (1024x1024 sprites)
        m_anim.frameSize = sf::Vector2u(341, 341); // Tamaño correcto para 1024x1024 / 3
        m_anim.columns = 3;
        m_anim.row = 0;
        m_anim.current = 0;
        m_anim.timer = 0.f;
        m_anim.frameDuration = 0.2f;
        
        // Asegurar que el sprite esté visible
        m_useSprite = true;
        std::cout << "[Entity] Animation reset to movement mode" << std::endl;
        
        // Si es el enemy, marcar que terminó su turno de combate
        if (m_type == EntityType::Enemy) {
            std::cout << "[Entity] Enemy terminó animación de combate, listo para terminar turno" << std::endl;
        }
    }
}
