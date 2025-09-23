#pragma once
#include <SFML/Graphics.hpp>

struct PlayerStats {
    int hp;
    int maxHp;
    int pa;
    int maxPa;
    int pm;
    int maxPm;
    
    PlayerStats() : hp(100), maxHp(100), pa(6), maxPa(6), pm(3), maxPm(3) {}
};

class HUD {
public:
    HUD();
    
    // Configuración de estadísticas
    void setPlayerStats(int hp, int maxHp, int pa, int maxPa, int pm, int maxPm);
    void setEnemyStats(int hp, int maxHp, int pa, int maxPa, int pm, int maxPm);
    void setTurn(bool isPlayerTurn);
    void setActiveSpellIndex(int index);
    
    // Renderizado
    void draw(sf::RenderTarget& target);
    
    // Configuración de ventana
    void setWindowSize(sf::Vector2u windowSize);
    
    // Configuración de escala virtual
    void setVirtualScale(float scale);
    
private:
    // Estadísticas
    PlayerStats m_playerStats;
    PlayerStats m_enemyStats;
    bool m_isPlayerTurn;
    int m_activeSpellIndex;
    
    // Dimensiones de ventana
    sf::Vector2u m_windowSize;
    
    // Escala virtual para responsive
    float m_virtualScale;
    
    // Colores
    static const sf::Color HP_COLOR;
    static const sf::Color PA_COLOR;
    static const sf::Color PM_COLOR;
    static const sf::Color PANEL_BG_COLOR;
    static const sf::Color PANEL_BORDER_COLOR;
    static const sf::Color TURN_GLOW_COLOR;
    static const sf::Color SPELL_SLOT_COLOR;
    static const sf::Color SPELL_SLOT_ACTIVE_COLOR;
    
    // Colores de hechizos
    static const sf::Color GOLPE_COLOR;
    static const sf::Color FLECHA_COLOR;
    static const sf::Color CURAR_COLOR;
    
    // Métodos de dibujo
    void drawPanel(sf::RenderTarget& target, float x, float y, float width, float height, bool isActive);
    void drawBar(sf::RenderTarget& target, float x, float y, float width, float height, float ratio, const sf::Color& fillColor);
    void drawSpellSlots(sf::RenderTarget& target, float x, float y);
    void drawTurnIndicator(sf::RenderTarget& target, float x, float y, float width, float height);
    
    // Helpers
    sf::Color getSpellColor(int spellIndex) const;
    sf::Vector2f getPanelPosition(bool isPlayer) const;
    sf::Vector2f getSpellSlotsPosition() const;
};
