#include "systems/Display.h"
#include <algorithm>
#include <iostream>
#include "map/Map.h"

namespace Display {
	static bool s_firstFrame = true;

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
		
		// Log de diagnóstico en el primer frame
		if (s_firstFrame) {
			std::cout << "[Display] Viewport created: window=" << winW << "x" << winH 
			          << ", virtual=" << VIRTUAL_W << "x" << VIRTUAL_H
			          << ", scale=" << scale 
			          << ", viewport=(" << vpL << "," << vpT << "," << vpW << "," << vpH << ")" << std::endl;
		}
		
		return view;
	}

	void applyLetterbox(sf::RenderWindow& w) {
		auto view = makeLetterboxedView(w.getSize());
		w.setView(view);
		
		// Log de diagnóstico en el primer frame
		if (s_firstFrame) {
			sf::FloatRect viewRect = view.getViewport();
			sf::Vector2f viewCenter = view.getCenter();
			sf::Vector2f viewSize = view.getSize();
			std::cout << "[Display] View applied: center=(" << viewCenter.x << "," << viewCenter.y << ")"
			          << ", size=(" << viewSize.x << "x" << viewSize.y << ")" << std::endl;
			s_firstFrame = false;
		}
	}

	void centerMapInView(Map& map) {
		static bool s_firstCenter = true;
		map.setCenteredOffset(sf::Vector2f(VIRTUAL_W, VIRTUAL_H));
		
		// Log de diagnóstico solo la primera vez
		if (s_firstCenter) {
			// Obtener offset del mapa para mostrarlo
			sf::Vector2f offset = map.getTileTopLeft(0, 0);
			std::cout << "[Display] Center OK: offset=(" << offset.x << "," << offset.y 
			          << "), view=" << VIRTUAL_W << "x" << VIRTUAL_H << std::endl;
			s_firstCenter = false;
		}
	}
}


