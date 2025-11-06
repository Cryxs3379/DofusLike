# DofusLike - Misión 2

## Descripción
Implementación de la Misión 2 del juego táctico estilo Dofus con C++ y SFML 3.0.1.

## Características Implementadas

### ✅ Tablero Isométrico
- Tablero de 15x15 losetas en forma de rombos
- Sistema de coordenadas isométricas completo
- Losetas verdes (libres) y rojas (bloqueadas)

### ✅ Interacción con el Mouse
- **Hover**: Resaltado amarillo de la loseta bajo el cursor
- **Clic Derecho**: Alternar entre libre/bloqueada
- **Clic Izquierdo**: Mover el peón a casillas alcanzables

### ✅ Sistema de Peón
- Peón azul con 3 PM (Puntos de Movimiento)
- Movimiento ortogonal (arriba, abajo, izquierda, derecha)
- Animación de movimiento suave (~0.18s por casilla)

### ✅ Pathfinding
- Algoritmo BFS para calcular casillas alcanzables
- Resaltado cian de las casillas alcanzables
- Recalculación automática al cambiar el mapa o posición del peón

## Controles
- **Mouse**: Mover para resaltar losetas
- **Clic Derecho**: Alternar loseta libre/bloqueada
- **Clic Izquierdo**: Mover peón a casilla alcanzable

## Estructura del Código

### Módulos Implementados
- **`App`**: Aplicación principal y bucle de juego
- **`Map`**: Gestión del tablero isométrico
- **`Isometric`**: Utilidades de coordenadas isométricas
- **`Pathfinding`**: Algoritmo BFS para movimiento
- **`Pawn`**: Lógica del peón y animaciones

### APIs de SFML 3 Utilizadas
- `pollEvent()` devuelve `std::optional<sf::Event>`
- `sf::Mouse::Button::Left/Right`
- `setOrigin({x,y})`
- `sf::VideoMode({width, height})`

## Compilación
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

## Ejecución
```bash
./DofusLike.exe
```

## Notas Técnicas
- El peón inicia en el centro del mapa (7,7)
- Las casillas alcanzables se recalculan automáticamente
- El movimiento es paso a paso con interpolación suave
- Sistema de coordenadas isométricas optimizado para rombos
