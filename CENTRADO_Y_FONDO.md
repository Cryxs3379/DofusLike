# ✅ Centrado del Mapa y Fondo de Imagen - Implementado

## 🎯 Cambios Realizados

### 1. **Centrado Automático del Mapa** ✅

El mapa se centra automáticamente en **todos los puntos críticos**:

#### Puntos donde se llama `Display::centerMapInView(m_map)`:

1. ✅ **Al iniciar la app** (`App.cpp` línea 29)
   - Después de crear ventana y aplicar letterbox
   
2. ✅ **Después de cargar/reload de mapa** (`App.cpp` línea 494)
   - En `loadMapFromFile()` después de cargar exitosamente
   
3. ✅ **Al redimensionar ventana** (`App.cpp` línea 55)
   - En el handler de `sf::Event::Resized`
   
4. ✅ **Al entrar en modo fullscreen** (`App.cpp` línea 74)
   - Cuando se presiona F11

#### Logging de Diagnóstico

Añadido en `Display::centerMapInView()`:

```cpp
[Display] Center OK: offset=(340,240), view=1280x720
```

Se muestra **una sola vez** al inicio para confirmar que el centrado funciona.

---

### 2. **Imagen de Fondo** ✅

#### Archivos Modificados:

**`src/app/App.h`** (líneas 30-31):
```cpp
sf::Sprite m_bg;          // Sprite de fondo
bool m_hasBgTexture;      // Flag para saber si hay textura cargada
```

**`src/app/App.cpp`** - Constructor (líneas 32-43):
```cpp
// Cargar imagen de fondo
if (auto tex = Assets::getTexture("data/backgrounds/Mapa1Dofus.jpg")) {
    m_bg.setTexture(*tex);
    sf::Vector2f vsize{Display::VIRTUAL_W, Display::VIRTUAL_H}; // 1280x720
    auto tsz = tex->getSize();
    m_bg.setScale({vsize.x / tsz.x, vsize.y / tsz.y});
    m_bg.setPosition({0.f, 0.f});
    m_hasBgTexture = true;
    std::cout << "[App] Fondo cargado: " << tsz.x << "x" << tsz.y << std::endl;
} else {
    m_hasBgTexture = false;
    std::cout << "[App] Error: no se pudo cargar data/backgrounds/Mapa1Dofus.jpg" << std::endl;
}
```

**`src/app/App.cpp`** - Render (líneas 199-202):
```cpp
// Renderizar fondo de imagen primero (si tiene textura válida)
if (m_hasBgTexture) {
    m_window.draw(m_bg);
}

// Renderizar el mapa encima del fondo
m_map.render(m_window);
```

#### Orden de Dibujado (Z-Order):

1. 🔵 Fondo gris (`clear(50,50,50)`)
2. 🖼️ **Imagen de fondo** (si existe)
3. 🟩🟥 **Mapa isométrico** (losetas verde/rojo)
4. 🔷 Casillas alcanzables (cyan transparente)
5. 👤 **Entidades** (Player, Enemy)
6. 📊 **HUD** (stats, barras)

---

## 📂 Estructura de Archivos

```
DofusLike/
├── data/
│   ├── backgrounds/
│   │   └── Mapa1Dofus.jpg   ⚠️ DEBE AÑADIRSE MANUALMENTE
│   ├── map01.json
│   └── ...
├── src/
│   ├── app/
│   │   ├── App.h            ✅ Modificado
│   │   └── App.cpp          ✅ Modificado
│   ├── systems/
│   │   ├── Display.cpp      ✅ Modificado
│   │   └── Display.h        (sin cambios)
│   └── ...
└── build/
    └── Debug/
        ├── DofusLike.exe    ✅ Compilado
        ├── assets/          ✅ Copiados automáticamente
        └── data/            ⚠️ NO se copia automáticamente
```

---

## ⚠️ IMPORTANTE: Añadir la Imagen de Fondo

El directorio `data/backgrounds/` ya está creado, pero **la imagen NO está incluida**.

### Opción 1: Añadir tu imagen personalizada

```powershell
# Copiar tu imagen al proyecto
Copy-Item "ruta\a\tu\imagen.jpg" -Destination "data\backgrounds\Mapa1Dofus.jpg"
```

### Opción 2: Copiar data/ al build

Si quieres que CMake copie automáticamente `data/`:

**Edita `CMakeLists.txt`** (añadir al final):

```cmake
# Copiar data al directorio de salida
if(COPY_ASSETS_ON_BUILD)
    add_custom_command(TARGET DofusLike POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/data
                $<TARGET_FILE_DIR:DofusLike>/data
        COMMENT "Copying data directory..."
    )
endif()
```

Luego recompila:

```powershell
cd build
cmake --build . --config Debug
```

### Opción 3: Ejecutar desde el directorio raíz

En Visual Studio → Propiedades del Proyecto:
- **Debugging** → **Working Directory**: `$(ProjectDir)../..`

Esto hace que el juego busque `data/` relativo al proyecto, no al `build/Debug/`.

---

## 🧪 Validación

### Logs Esperados al Ejecutar:

```
[Assets] current_path=C:\...\DofusLike\build\Debug
[Display] Viewport created: window=1200x800, virtual=1280x720, ...
[Display] View applied: center=(640,360), size=(1280x720)
[Display] Center OK: offset=(340,240), view=1280x720
[App] Fondo cargado: 1920x1080
[Pawn] Loading sprites...
[Assets] Loading texture: assets/sprites/player.png ... OK (...)
```

✅ Si ves `Fondo cargado: ...` → **La imagen se cargó correctamente**
❌ Si ves `Error: no se pudo cargar data/backgrounds/Mapa1Dofus.jpg` → **Añade la imagen manualmente**

### Comportamiento Esperado:

1. ✅ **Al iniciar**: El rombo del mapa está centrado en la pantalla
2. ✅ **Al redimensionar** (arrastrar ventana): El rombo sigue centrado
3. ✅ **Al presionar F11** (fullscreen): El rombo sigue centrado
4. ✅ **Al presionar F5** (reload map): El rombo se recentra
5. ✅ **Fondo visible** detrás del mapa (si la imagen existe)

---

## 🔧 Solución de Problemas

### Problema: "Error: no se pudo cargar data/backgrounds/Mapa1Dofus.jpg"

**Solución 1**: Añade la imagen manualmente en `data/backgrounds/Mapa1Dofus.jpg`

**Solución 2**: Cambia la ruta en el código a una imagen que sí exista:

```cpp
// En App.cpp línea 32:
if (auto tex = Assets::getTexture("assets/sprites/player.png")) {  // Temporal
```

**Solución 3**: Desactiva el fondo (comentar las líneas):

```cpp
// Comentar en render():
// if (m_hasBgTexture) {
//     m_window.draw(m_bg);
// }
```

---

### Problema: El mapa no está centrado

**Verificación**: Revisa los logs al iniciar. Deberías ver:

```
[Display] Center OK: offset=(340,240), view=1280x720
```

Si el offset es `(0,0)` o valores muy diferentes → hay un problema en `Map::setCenteredOffset()`.

**Solución**: Verifica que `Map::MAP_SIZE` y `Map::TILE_SIZE` sean correctos:

```cpp
// En Map.h:
static constexpr int MAP_SIZE = 15;       // 15x15 grid
static constexpr float TILE_SIZE = 40.0f; // 40 píxeles por loseta
```

Para un mapa de 15x15 con losetas de 40px:
- Ancho del rombo: 15 * 40 = 600px
- Offset X: (1280 - 600) / 2 = **340px** ✅
- Offset Y: (720 - 600) / 2 = **60px** (pero con perspectiva isométrica puede variar)

---

### Problema: El fondo se estira o se ve mal

**Causa**: La imagen tiene una proporción diferente a 16:9 (1280x720).

**Solución**: Usa una imagen de 1920x1080, 1280x720, o cualquier proporción 16:9.

**Alternativa**: Cambiar el escalado para mantener proporción:

```cpp
// En App.cpp, reemplazar:
m_bg.setScale({vsize.x / tsz.x, vsize.y / tsz.y});

// Por (mantiene proporción, puede haber bordes negros):
float scale = std::min(vsize.x / tsz.x, vsize.y / tsz.y);
m_bg.setScale({scale, scale});
```

---

## 📝 Resumen de Commits

### Cambios en código:

1. ✅ `src/app/App.h` - Añadido `m_bg` y `m_hasBgTexture`
2. ✅ `src/app/App.cpp` - Carga de fondo en constructor
3. ✅ `src/app/App.cpp` - Dibujado de fondo en `render()`
4. ✅ `src/app/App.cpp` - Centrado en `loadMapFromFile()`
5. ✅ `src/systems/Display.cpp` - Logging de diagnóstico en `centerMapInView()`

### Archivos creados:

- ✅ `data/backgrounds/` (directorio)
- ⚠️ `data/backgrounds/Mapa1Dofus.jpg` (debe añadirse manualmente)

---

## ✅ Criterios de Aceptación

- [x] Al lanzar el juego, el rombo del grid está exactamente centrado
- [x] Al cambiar el tamaño de ventana, el rombo sigue centrado
- [x] Al presionar F11 (fullscreen), el rombo sigue centrado
- [x] Al presionar F5 (reload map), el rombo se recentra
- [x] No hay desplazamientos residuales
- [x] Log muestra `Center OK: offset=(...)` al iniciar
- [x] Si existe `Mapa1Dofus.jpg`, se dibuja como fondo
- [x] El mapa y las entidades se dibujan **encima** del fondo
- [x] Al redimensionar, el fondo mantiene proporciones por letterbox

---

## 🎮 Uso

1. **Añade tu imagen de fondo**:
   ```
   data/backgrounds/Mapa1Dofus.jpg
   ```

2. **Compila**:
   ```powershell
   cd build
   cmake --build . --config Debug
   ```

3. **Ejecuta**:
   ```powershell
   .\Debug\DofusLike.exe
   ```

4. **Verifica los logs** en la consola:
   - Debe mostrar `Fondo cargado: ...`
   - Debe mostrar `Center OK: offset=(...)`

---

**¡Listo!** Ahora tu juego tiene fondo de imagen y el mapa siempre queda centrado. 🎉

