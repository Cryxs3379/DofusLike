#include "systems/Display.h"
#include <algorithm>
#include "map/Map.h"

namespace Display {
	sf::View makeLetterboxedView(sf::Vector2u win) {
		if (win.x == 0u || win.y == 0u) {
			return sf::View(sf::FloatRect({0.f, 0.f}, {VIRTUAL_W, VIRTUAL_H}));
		}
		sf::View view(sf::FloatRect({0.f, 0.f}, {VIRTUAL_W, VIRTUAL_H}));
		const float winW = static_cast<float>(win.x);
		const float winH = static_cast<float>(win.y);
		const float scale = std::min(winW / VIRTUAL_W, winH / VIRTUAL_H);
		const float vpW = (VIRTUAL_W * scale) / winW;
		const float vpH = (VIRTUAL_H * scale) / winH;
		const float vpL = (1.f - vpW) * 0.5f;
		const float vpT = (1.f - vpH) * 0.5f;
		view.setViewport(sf::FloatRect({vpL, vpT}, {vpW, vpH}));
		return view;
	}

	void applyLetterbox(sf::RenderWindow& w) {
		auto view = makeLetterboxedView(w.getSize());
		w.setView(view);
	}

	void centerMapInView(Map& map) {
		map.setCenteredOffset(sf::Vector2f(VIRTUAL_W, VIRTUAL_H));
	}
}


