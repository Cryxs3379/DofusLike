#include "systems/HUD.h"
#include <algorithm>

// Definir colores estáticos
const sf::Color HUD::HP_COLOR = sf::Color(220, 20, 20);           // Rojo
const sf::Color HUD::PA_COLOR = sf::Color(255, 140, 0);           // Naranja
const sf::Color HUD::PM_COLOR = sf::Color(30, 144, 255);          // Azul
const sf::Color HUD::PANEL_BG_COLOR = sf::Color(40, 40, 40, 200); // Gris oscuro semi-transparente
const sf::Color HUD::PANEL_BORDER_COLOR = sf::Color(100, 100, 100); // Gris claro
const sf::Color HUD::TURN_GLOW_COLOR = sf::Color(255, 255, 0, 100); // Amarillo semi-transparente
const sf::Color HUD::SPELL_SLOT_COLOR = sf::Color(60, 60, 60);    // Gris oscuro
const sf::Color HUD::SPELL_SLOT_ACTIVE_COLOR = sf::Color(255, 255, 255); // Blanco

// Colores de hechizos
const sf::Color HUD::GOLPE_COLOR = sf::Color(220, 20, 20);        // Rojo
const sf::Color HUD::FLECHA_COLOR = sf::Color(20, 220, 20);       // Verde
const sf::Color HUD::CURAR_COLOR = sf::Color(255, 255, 0);        // Amarillo

HUD::HUD() : m_isPlayerTurn(true), m_activeSpellIndex(0), m_windowSize(1280, 720), m_virtualScale(1.0f) {
}

void HUD::setPlayerStats(int hp, int maxHp, int pa, int maxPa, int pm, int maxPm) {
    m_playerStats.hp = std::max(0, std::min(hp, maxHp));
    m_playerStats.maxHp = std::max(1, maxHp);
    m_playerStats.pa = std::max(0, std::min(pa, maxPa));
    m_playerStats.maxPa = std::max(1, maxPa);
    m_playerStats.pm = std::max(0, std::min(pm, maxPm));
    m_playerStats.maxPm = std::max(1, maxPm);
}

void HUD::setEnemyStats(int hp, int maxHp, int pa, int maxPa, int pm, int maxPm) {
    m_enemyStats.hp = std::max(0, std::min(hp, maxHp));
    m_enemyStats.maxHp = std::max(1, maxHp);
    m_enemyStats.pa = std::max(0, std::min(pa, maxPa));
    m_enemyStats.maxPa = std::max(1, maxPa);
    m_enemyStats.pm = std::max(0, std::min(pm, maxPm));
    m_enemyStats.maxPm = std::max(1, maxPm);
}

void HUD::setTurn(bool isPlayerTurn) {
    m_isPlayerTurn = isPlayerTurn;
}

void HUD::setActiveSpellIndex(int index) {
    m_activeSpellIndex = std::max(0, std::min(index, 2)); // 0-2 para los 3 hechizos
}

void HUD::setWindowSize(sf::Vector2u windowSize) {
    m_windowSize = windowSize;
}

void HUD::setVirtualScale(float scale) {
    m_virtualScale = scale;
}

void HUD::draw(sf::RenderTarget& target) {
    // Dimensiones del HUD basadas en resolución virtual (1280x720)
    const float VIRTUAL_WIDTH = 1280.0f;
    const float VIRTUAL_HEIGHT = 720.0f;
    
    // Dimensiones en unidades virtuales (la View se encarga del escalado)
    float hudHeight = 120.0f;
    float panelWidth = VIRTUAL_WIDTH * 0.4f; // 40% del ancho virtual
    float panelHeight = 80.0f;
    float panelSpacing = VIRTUAL_WIDTH * 0.1f; // 10% de separación
    
    // Posiciones de los paneles (coordenadas virtuales)
    float leftPanelX = panelSpacing;
    float rightPanelX = VIRTUAL_WIDTH - panelSpacing - panelWidth;
    float panelY = 20.0f;
    
    // Dibujar panel del jugador
    drawPanel(target, leftPanelX, panelY, panelWidth, panelHeight, m_isPlayerTurn);
    
    // Dibujar barras del jugador
    float barWidth = panelWidth - 20.0f;
    float barHeight = 12.0f;
    float barSpacing = 15.0f;
    float barX = leftPanelX + 10.0f;
    float barY = panelY + 10.0f;
    
    // HP del jugador
    float hpRatio = static_cast<float>(m_playerStats.hp) / static_cast<float>(m_playerStats.maxHp);
    drawBar(target, barX, barY, barWidth, barHeight, hpRatio, HP_COLOR);
    
    // PA del jugador
    float paRatio = static_cast<float>(m_playerStats.pa) / static_cast<float>(m_playerStats.maxPa);
    drawBar(target, barX, barY + barSpacing, barWidth, barHeight, paRatio, PA_COLOR);
    
    // PM del jugador
    float pmRatio = static_cast<float>(m_playerStats.pm) / static_cast<float>(m_playerStats.maxPm);
    drawBar(target, barX, barY + barSpacing * 2, barWidth, barHeight, pmRatio, PM_COLOR);
    
    // Dibujar panel del enemigo
    drawPanel(target, rightPanelX, panelY, panelWidth, panelHeight, !m_isPlayerTurn);
    
    // Dibujar barras del enemigo
    barX = rightPanelX + 10.0f;
    
    // HP del enemigo
    float enemyHpRatio = static_cast<float>(m_enemyStats.hp) / static_cast<float>(m_enemyStats.maxHp);
    drawBar(target, barX, barY, barWidth, barHeight, enemyHpRatio, HP_COLOR);
    
    // PA del enemigo
    float enemyPaRatio = static_cast<float>(m_enemyStats.pa) / static_cast<float>(m_enemyStats.maxPa);
    drawBar(target, barX, barY + barSpacing, barWidth, barHeight, enemyPaRatio, PA_COLOR);
    
    // PM del enemigo
    float enemyPmRatio = static_cast<float>(m_enemyStats.pm) / static_cast<float>(m_enemyStats.maxPm);
    drawBar(target, barX, barY + barSpacing * 2, barWidth, barHeight, enemyPmRatio, PM_COLOR);
    
    // Dibujar slots de hechizos (solo para el jugador)
    if (m_isPlayerTurn) {
        float slotsX = leftPanelX;
        float slotsY = panelY + panelHeight + 10.0f;
        drawSpellSlots(target, slotsX, slotsY);
    }
}

void HUD::drawPanel(sf::RenderTarget& target, float x, float y, float width, float height, bool isActive) {
    // Dibujar glow de turno si está activo
    if (isActive) {
        float glowOffset = 5.0f;
        drawTurnIndicator(target, x - glowOffset, y - glowOffset, width + glowOffset * 2, height + glowOffset * 2);
    }
    
    // Fondo del panel
    sf::RectangleShape panel(sf::Vector2f(width, height));
    panel.setPosition({x, y});
    panel.setFillColor(PANEL_BG_COLOR);
    panel.setOutlineColor(PANEL_BORDER_COLOR);
    panel.setOutlineThickness(2.0f);
    target.draw(panel);
}

void HUD::drawBar(sf::RenderTarget& target, float x, float y, float width, float height, float ratio, const sf::Color& fillColor) {
    // Fondo de la barra
    sf::RectangleShape barBg(sf::Vector2f(width, height));
    barBg.setPosition({x, y});
    barBg.setFillColor(sf::Color(20, 20, 20)); // Negro oscuro
    barBg.setOutlineColor(sf::Color(100, 100, 100));
    barBg.setOutlineThickness(1.0f);
    target.draw(barBg);
    
    // Barra de relleno
    if (ratio > 0.0f) {
        float fillWidth = width * std::max(0.0f, std::min(1.0f, ratio));
        sf::RectangleShape barFill(sf::Vector2f(fillWidth, height));
        barFill.setPosition({x, y});
        barFill.setFillColor(fillColor);
        target.draw(barFill);
    }
}

void HUD::drawSpellSlots(sf::RenderTarget& target, float x, float y) {
    float slotWidth = 30.0f;
    float slotHeight = 20.0f;
    float slotSpacing = 5.0f;
    
    for (int i = 0; i < 3; ++i) {
        float slotX = x + i * (slotWidth + slotSpacing);
        
        // Fondo del slot
        sf::RectangleShape slot(sf::Vector2f(slotWidth, slotHeight));
        slot.setPosition({slotX, y});
        slot.setFillColor(SPELL_SLOT_COLOR);
        
        // Borde del slot
        sf::Color borderColor = (i == m_activeSpellIndex) ? SPELL_SLOT_ACTIVE_COLOR : sf::Color(100, 100, 100);
        float borderThickness = (i == m_activeSpellIndex) ? 3.0f : 1.0f;
        slot.setOutlineColor(borderColor);
        slot.setOutlineThickness(borderThickness);
        target.draw(slot);
        
        // Color del hechizo (pequeño rectángulo interno)
        float innerOffset = 2.0f;
        sf::RectangleShape spellColor(sf::Vector2f(slotWidth - innerOffset * 2.0f, slotHeight - innerOffset * 2.0f));
        spellColor.setPosition({slotX + innerOffset, y + innerOffset});
        spellColor.setFillColor(getSpellColor(i));
        target.draw(spellColor);
    }
}

void HUD::drawTurnIndicator(sf::RenderTarget& target, float x, float y, float width, float height) {
    sf::RectangleShape glow(sf::Vector2f(width, height));
    glow.setPosition({x, y});
    glow.setFillColor(TURN_GLOW_COLOR);
    target.draw(glow);
}

sf::Color HUD::getSpellColor(int spellIndex) const {
    switch (spellIndex) {
        case 0: return GOLPE_COLOR;    // Rojo
        case 1: return FLECHA_COLOR;   // Verde
        case 2: return CURAR_COLOR;    // Amarillo
        default: return sf::Color::White;
    }
}

sf::Vector2f HUD::getPanelPosition(bool isPlayer) const {
    const float VIRTUAL_WIDTH = 1280.0f;
    float panelWidth = VIRTUAL_WIDTH * 0.4f;
    float panelSpacing = VIRTUAL_WIDTH * 0.1f;
    
    if (isPlayer) {
        return sf::Vector2f(panelSpacing, 20.0f);
    } else {
        return sf::Vector2f(VIRTUAL_WIDTH - panelSpacing - panelWidth, 20.0f);
    }
}

sf::Vector2f HUD::getSpellSlotsPosition() const {
    const float VIRTUAL_WIDTH = 1280.0f;
    float panelSpacing = VIRTUAL_WIDTH * 0.1f;
    return sf::Vector2f(panelSpacing, (20.0f + 80.0f + 10.0f));
}
