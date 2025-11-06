#pragma once
#include <SFML/Graphics.hpp>

struct Animation {
    sf::Vector2u frameSize;      // Tamaño de cada frame
    unsigned int columns;        // Número de columnas en el spritesheet
    unsigned int row;            // Fila actual (0=idle, 1=up, 2=left, 3=down, 4=right)
    unsigned int current;        // Frame actual
    float timer;                 // Timer para cambio de frame
    float frameDuration;         // Duración de cada frame en segundos
    
    // Constructor
    Animation() : frameSize(32, 32), columns(4), row(0), current(0), timer(0.0f), frameDuration(0.2f) {}
    
    // Actualizar animación
    void update(float deltaTime);
    
    // Aplicar frame actual al sprite
    void apply(sf::Sprite& sprite);
    
    // Cambiar dirección (0=idle, 1=up, 2=left, 3=down, 4=right)
    void setDirection(int direction);
    
    // Resetear a frame 0
    void reset();
};
