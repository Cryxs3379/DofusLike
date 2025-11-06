# Diagnóstico y Solución: Personajes No Visibles

## 🔍 Qué Estaba Mal

El problema principal era que **las texturas se cargaban usando rutas relativas simples** (ej: `"assets/sprites/player.png"`), pero dependiendo del **directorio de trabajo** (working directory) actual al ejecutar el juego, estas rutas podían no encontrarse.

### Síntomas:
- ✅ Antes funcionaba (cuando se ejecutaba desde el directorio del proyecto)
- ❌ Ahora no se ven los personajes (al ejecutar desde otro directorio o desde Visual Studio con working directory diferente)
- ❌ Las texturas fallaban al cargar silenciosamente
- ❌ Los personajes caían en el fallback de círculo pequeño (radio 8px) que podía quedar fuera de cámara

## ✅ Qué Se Arregló

### 1. **Sistema Robusto de Búsqueda de Assets** (`Assets.cpp/h`)

Ahora el sistema busca las texturas en **múltiples ubicaciones** automáticamente:

#### Orden de búsqueda:
1. ✨ Variable de entorno `ASSET_ROOT` (si existe)
2. ✨ Ruta personalizada via `Assets::setRoot()` (si se estableció)
3. 📁 Directorio de trabajo actual (`current_path()`)
4. 📁 Directorio padre (`current_path()/..`)
5. 📁 Dos niveles arriba (`current_path()/../..`)
6. 📁 Subdirectorio `bin/`
7. 📁 Subdirectorio `build/`
8. 📁 Subdirectorio `build/Debug/` (común en Visual Studio)

#### Nuevo método público:
```cpp
Assets::setRoot(std::filesystem::path root)
```
Permite establecer manualmente la ruta base donde buscar assets.

### 2. **Logging Detallado y Claro**

Ahora verás en consola **exactamente** qué está pasando:

```
[Assets] current_path=C:\Users\...\DofusLike\build\Debug
[Assets] Loading texture: assets/sprites/player.png ... OK (found at: C:\Users\...\DofusLike\build\Debug\assets\sprites\player.png, size=720x330)
```

o si falla:

```
[Assets] Loading texture: assets/sprites/missing.png ... FAIL (searched from C:\Users\...\DofusLike\build\Debug and parent directories)
```

### 3. **Opción de Diagnóstico Visual** (`Entity.h/cpp` y `Pawn.h/cpp`)

Añadida flag global `FORCE_DEBUG_CIRCLE` tanto para `Entity` como para `Pawn`:

```cpp
// En main.cpp o al inicio de App.cpp, ANTES de crear entidades/pawns:
Entity::FORCE_DEBUG_CIRCLE = true;
Pawn::FORCE_DEBUG_CIRCLE = true;
```

**Efecto:** Muestra círculos grandes (radio 16px) en lugar de sprites, para confirmar que el renderizado funciona independientemente de los assets.

### 4. **Logging de Viewport** (`Display.cpp`)

En el primer frame, se imprime información del viewport y cámara:

```
[Display] Viewport created: window=1280x720, virtual=1280x720, scale=1, viewport=(0,0,1,1)
[Display] View applied: center=(640,360), size=(1280x720)
```

Esto descarta que los personajes estén fuera de la cámara.

### 5. **CMake Mejorado** (`CMakeLists.txt`)

Añadida opción configurable:

```cmake
option(COPY_ASSETS_ON_BUILD "Copy assets to build output directory automatically" ON)
```

Por defecto **ACTIVA** (copia assets a `build/Debug/assets/` automáticamente tras compilar).

## 🚀 Cómo Usar

### **Opción A: Confiar en la Copia Automática (Recomendado)**

1. **Reconfigurar CMake** (solo si cambiaste la opción):
   ```bash
   cd build
   cmake .. -DCOPY_ASSETS_ON_BUILD=ON
   ```

2. **Compilar** (automáticamente copia assets):
   ```bash
   cmake --build . --config Debug
   ```

3. **Ejecutar** desde cualquier directorio de trabajo:
   ```bash
   build\Debug\DofusLike.exe
   ```

✅ **Funcionará** porque los assets están en `build/Debug/assets/`

---

### **Opción B: Variable de Entorno `ASSET_ROOT`**

1. **Establecer variable de entorno** (Windows PowerShell):
   ```powershell
   $env:ASSET_ROOT = "C:\Users\PTRUJILLO\Desktop\Trujillo\ProyectosCryxs\DofusLike"
   .\build\Debug\DofusLike.exe
   ```

2. O configurarla permanentemente en Visual Studio:
   - Proyecto → Propiedades → Debugging → Environment
   - Añadir: `ASSET_ROOT=C:\Users\PTRUJILLO\Desktop\Trujillo\ProyectosCryxs\DofusLike`

---

### **Opción C: Establecer Ruta Manualmente en Código**

En `src/main.cpp` o `src/app/App.cpp`, **ANTES** de crear entidades:

```cpp
#include "systems/Assets.h"

int main() {
    // Establecer ruta absoluta al directorio del proyecto
    Assets::setRoot("C:/Users/PTRUJILLO/Desktop/Trujillo/ProyectosCryxs/DofusLike");
    
    // ... resto del código
}
```

---

## 🧪 Validación Manual

### Prueba 1: Verificar Carga de Texturas

**Ejecuta el juego** y revisa la consola. Deberías ver:

```
[Assets] current_path=...
[Assets] Loading texture: assets/sprites/player.png ... OK (found at: ..., size=720x330)
[Entity] Loading sprites for player...
[Entity] Main texture: LOADED
```

✅ Si ves `OK` → las texturas se cargan correctamente.
❌ Si ves `FAIL` → revisa las opciones A, B o C arriba.

---

### Prueba 2: Forzar Error para Verificar Logging

**Renombra temporalmente** `assets/sprites/player.png` a `player_backup.png`.

**Ejecuta el juego** → Deberías ver:

```
[Assets] Loading texture: assets/sprites/player.png ... FAIL (searched from ... and parent directories)
[Entity] sprite OFF (fallback)
```

✅ Confirma que el logging funciona.
🔄 **Restaura el nombre** del archivo.

---

### Prueba 3: Modo Debug de Círculos

En `src/main.cpp` o `src/app/App.cpp`, añade **temporalmente**:

```cpp
#include "units/Entity.h"
#include "units/Pawn.h"

int main() {
    // Activar modo debug antes de crear personajes
    Entity::FORCE_DEBUG_CIRCLE = true;
    Pawn::FORCE_DEBUG_CIRCLE = true;
    
    // ... resto del código (crear App, etc.)
}
```

**Compila y ejecuta** → Deberías ver **círculos grandes azules** en lugar de sprites.

✅ Si ves los círculos → el renderizado funciona (el problema es de assets).
❌ Si no ves nada → problema de cámara/viewport (revisa logs de Display).

🔄 **Quita o comenta** las líneas después de la prueba.

---

## 📝 Resumen de Cambios en Código

### Archivos Modificados:

1. ✅ `src/systems/Assets.h` - Añadido `setRoot()` y `tryLoadTexture()`
2. ✅ `src/systems/Assets.cpp` - Sistema de búsqueda multi-ruta y logging mejorado
3. ✅ `src/units/Entity.h` - Flag `FORCE_DEBUG_CIRCLE`
4. ✅ `src/units/Entity.cpp` - Soporte para modo debug de círculos
5. ✅ `src/units/Pawn.h` - Flag `FORCE_DEBUG_CIRCLE`
6. ✅ `src/units/Pawn.cpp` - Soporte para modo debug de círculos
7. ✅ `src/systems/Display.cpp` - Logging de viewport en primer frame
8. ✅ `CMakeLists.txt` - Opción `COPY_ASSETS_ON_BUILD`

### Archivos Nuevos:

- 📄 `DIAGNOSTICO_SPRITES.md` (este documento)

---

## 🐛 Solución de Problemas

### Problema: "No se ven los personajes aún"

**Checklist:**
1. ✅ ¿Compilaste con `-DCOPY_ASSETS_ON_BUILD=ON`?
2. ✅ ¿Existe `build/Debug/assets/sprites/player.png`?
3. ✅ ¿La consola muestra `OK` al cargar texturas?
4. ✅ ¿`Entity::FORCE_DEBUG_CIRCLE = true` muestra círculos?

### Problema: "FAIL al cargar texturas"

**Soluciones:**
- Verifica que `assets/sprites/player.png` exista en el proyecto
- Usa `Assets::setRoot()` con ruta absoluta
- Establece `ASSET_ROOT` en variables de entorno
- Recompila con `cmake --build . --config Debug`

### Problema: "Los círculos tampoco se ven"

**Posibles causas:**
- Personajes fuera de cámara (revisa logs de `[Display]`)
- Problema de renderizado (verifica que `render()` se llame)
- Z-order (personajes dibujados detrás del mapa)

---

## 💡 Recomendación Final

La solución más robusta es **Opción A** (copia automática de assets) porque:
- ✅ Funciona desde cualquier working directory
- ✅ No requiere configuración manual
- ✅ Compatible con Visual Studio, VS Code y línea de comandos
- ✅ Los assets viajan con el ejecutable

Si ejecutas el juego **desde Visual Studio**, asegúrate de que el working directory sea `$(TargetDir)` o usa las opciones B/C.

---

**¡Listo!** Ahora tu juego debería mostrar los personajes correctamente sin importar desde dónde lo ejecutes. 🎮✨

