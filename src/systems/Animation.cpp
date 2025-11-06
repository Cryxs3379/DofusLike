#include "systems/Animation.h"

void Animation::update(float deltaTime) {
    timer += deltaTime;
    
    if (timer >= frameDuration) {
        timer = 0.0f;
        current = (current + 1) % columns;
    }
}

void Animation::apply(sf::Sprite& sprite) {
    // Si no hay spritesheet (1 frame), no modificar textureRect
    if (frameSize.x == 0 || frameSize.y == 0) {
        return; // Deja que el sprite use la textura completa
    }
    
    // calcular subrect del frame actual
    const int left = static_cast<int>(current * frameSize.x);
    const int top  = static_cast<int>(row * frameSize.y);
    const int w    = static_cast<int>(frameSize.x);
    const int h    = static_cast<int>(frameSize.y);
    
    // Solo aplicar si el rect es válido
    if (w > 0 && h > 0) {
        sprite.setTextureRect(sf::IntRect({left, top}, {w, h}));
    }
}

void Animation::setDirection(int direction) {
    if (direction >= 0 && direction <= 4) {
        row = static_cast<unsigned int>(direction);
        current = 0; // Resetear al primer frame de la dirección
        timer = 0.0f;
    }
}

void Animation::reset() {
    current = 0;
    timer = 0.0f;
    row = 0; // Volver a idle
}
