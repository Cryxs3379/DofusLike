#include "systems/Spells.h"
#include <iostream>

// Definir las listas estáticas de hechizos
std::vector<Spell> Spells::s_playerSpells;
std::vector<Spell> Spells::s_enemySpells;

const std::vector<Spell>& Spells::getPlayerSpells() {
    if (s_playerSpells.empty()) {
        initializeSpells();
    }
    return s_playerSpells;
}

const std::vector<Spell>& Spells::getEnemySpells() {
    if (s_enemySpells.empty()) {
        initializeSpells();
    }
    return s_enemySpells;
}

const Spell* Spells::getSpellByName(const std::string& name) {
    const auto& spells = getPlayerSpells();
    for (const auto& spell : spells) {
        if (spell.name == name) {
            return &spell;
        }
    }
    return nullptr;
}

const Spell* Spells::getSpellByIndex(int index) {
    const auto& spells = getPlayerSpells();
    if (index >= 0 && index < static_cast<int>(spells.size())) {
        return &spells[index];
    }
    return nullptr;
}

int Spells::getSpellCount() {
    return static_cast<int>(getPlayerSpells().size());
}

void Spells::initializeSpells() {
    // Hechizos del jugador
    s_playerSpells = {
        Spell("Golpe", 3, 1, 3, true, EffectType::Damage, 20, sf::Color::Red),
        Spell("Flecha", 4, 2, 5, true, EffectType::Damage, 15, sf::Color::Green),
        Spell("Curar", 2, 1, 3, true, EffectType::Heal, 15, sf::Color::Yellow)
    };
    
    // Hechizos del enemigo (solo Golpe por ahora)
    s_enemySpells = {
        Spell("Golpe", 3, 1, 3, true, EffectType::Damage, 20, sf::Color::Red)
    };
    
    std::cout << "Hechizos inicializados: " << s_playerSpells.size() << " para jugador, " 
              << s_enemySpells.size() << " para enemigo" << std::endl;
}
