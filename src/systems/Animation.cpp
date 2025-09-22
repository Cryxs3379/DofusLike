#include "systems/Animation.h"

void Animation::update(float deltaTime) {
    timer += deltaTime;
    
    if (timer >= frameDuration) {
        timer = 0.0f;
        current = (current + 1) % columns;
    }
}

void Animation::apply(sf::Sprite& sprite) {
    // Calcular posición del frame en el spritesheet
    int left = current * frameSize.x;
    int top = row * frameSize.y;
    
    // Aplicar textura rect al sprite
    sprite.setTextureRect(sf::IntRect{left, top, static_cast<int>(frameSize.x), static_cast<int>(frameSize.y)});
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
