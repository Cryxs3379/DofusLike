#pragma once
#include <SFML/Graphics.hpp>
class Map;

namespace Display {
    inline constexpr float VIRTUAL_W = 1280.f;
    inline constexpr float VIRTUAL_H = 720.f;
	sf::View makeLetterboxedView(sf::Vector2u win);
	void applyLetterbox(sf::RenderWindow& w);
	void centerMapInView(Map& map);
}


