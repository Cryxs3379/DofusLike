#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

enum class EffectType {
    Damage,
    Heal
};

struct Spell {
    std::string name;
    int costPA;
    int minRange;
    int maxRange;
    bool needsLoS;
    EffectType effectType;
    int value;
    sf::Color color;
    
    Spell(const std::string& n, int cost, int minR, int maxR, bool los, EffectType type, int val, sf::Color col)
        : name(n), costPA(cost), minRange(minR), maxRange(maxR), needsLoS(los), effectType(type), value(val), color(col) {}
};

class Spells {
public:
    static const std::vector<Spell>& getPlayerSpells();
    static const std::vector<Spell>& getEnemySpells();
    static const Spell* getSpellByName(const std::string& name);
    static const Spell* getSpellByIndex(int index);
    static int getSpellCount();
    
private:
    static std::vector<Spell> s_playerSpells;
    static std::vector<Spell> s_enemySpells;
    static void initializeSpells();
};
