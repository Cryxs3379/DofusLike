// ============================================================================
// EJEMPLOS DE USO - DIAGNÓSTICO DE SPRITES
// ============================================================================
// Este archivo muestra diferentes formas de usar las nuevas características
// de diagnóstico y configuración de assets.
//
// IMPORTANTE: No es necesario compilar este archivo, solo copiar las partes
// que necesites en tu main.cpp o App.cpp
// ============================================================================

// ----------------------------------------------------------------------------
// OPCIÓN 1: Usar modo debug de círculos (para verificar que el render funciona)
// ----------------------------------------------------------------------------
// Copia esto en src/main.cpp ANTES de crear cualquier Entity o Pawn

#include "app/App.h"
#include "units/Entity.h"
#include "units/Pawn.h"

int main() {
    // ⚠️ ACTIVAR SOLO PARA DIAGNÓSTICO - Muestra círculos en vez de sprites
    Entity::FORCE_DEBUG_CIRCLE = true;
    Pawn::FORCE_DEBUG_CIRCLE = true;
    
    App app;
    app.run();
    return 0;
}

// Resultado esperado: Verás círculos azules grandes (radio 16px) en lugar de sprites
// Si ves los círculos → el render funciona, el problema es de carga de assets
// Si no ves nada → hay un problema de cámara/viewport


// ----------------------------------------------------------------------------
// OPCIÓN 2: Establecer ruta personalizada de assets manualmente
// ----------------------------------------------------------------------------
// Copia esto en src/main.cpp si quieres forzar una ruta específica

#include "app/App.h"
#include "systems/Assets.h"

int main() {
    // Establecer ruta absoluta al directorio del proyecto
    // 🔄 CAMBIA ESTA RUTA A LA DE TU PROYECTO
    Assets::setRoot("C:/Users/PTRUJILLO/Desktop/Trujillo/ProyectosCryxs/DofusLike");
    
    App app;
    app.run();
    return 0;
}

// Resultado esperado: Los assets se cargarán desde esa ruta base
// Ejemplo: buscará en C:/Users/.../DofusLike/assets/sprites/player.png


// ----------------------------------------------------------------------------
// OPCIÓN 3: Combinar diagnóstico + ruta personalizada
// ----------------------------------------------------------------------------
// Útil para debugging completo

#include "app/App.h"
#include "systems/Assets.h"
#include "units/Entity.h"
#include "units/Pawn.h"
#include <iostream>

int main() {
    std::cout << "=== MODO DEBUG ACTIVADO ===" << std::endl;
    
    // 1. Establecer ruta de assets
    Assets::setRoot("C:/Users/PTRUJILLO/Desktop/Trujillo/ProyectosCryxs/DofusLike");
    
    // 2. Activar modo debug de círculos (comentar para ver sprites)
    // Entity::FORCE_DEBUG_CIRCLE = true;
    // Pawn::FORCE_DEBUG_CIRCLE = true;
    
    App app;
    app.run();
    return 0;
}

// Resultado esperado:
// - Verás mensajes detallados de carga de assets
// - Si descomentas FORCE_DEBUG_CIRCLE, verás círculos
// - Si no, verás sprites (si se cargan correctamente)


// ----------------------------------------------------------------------------
// OPCIÓN 4: Usar variable de entorno ASSET_ROOT (PowerShell/CMD)
// ----------------------------------------------------------------------------
// No requiere cambios en el código, solo ejecutar:

// PowerShell:
// $env:ASSET_ROOT = "C:\Users\PTRUJILLO\Desktop\Trujillo\ProyectosCryxs\DofusLike"
// .\build\Debug\DofusLike.exe

// CMD:
// set ASSET_ROOT=C:\Users\PTRUJILLO\Desktop\Trujillo\ProyectosCryxs\DofusLike
// build\Debug\DofusLike.exe

// Linux/Mac:
// export ASSET_ROOT=/home/user/proyectos/DofusLike
// ./build/Debug/DofusLike


// ----------------------------------------------------------------------------
// OPCIÓN 5: Configurar en Visual Studio (Propiedades del Proyecto)
// ----------------------------------------------------------------------------
// 1. Click derecho en el proyecto → Properties
// 2. Configuration Properties → Debugging
// 3. Environment: añadir
//    ASSET_ROOT=C:\Users\PTRUJILLO\Desktop\Trujillo\ProyectosCryxs\DofusLike
// 4. Working Directory: dejar $(ProjectDir) o cambiar a $(TargetDir)


// ============================================================================
// VERIFICACIÓN DE LOGS ESPERADOS
// ============================================================================

// Al ejecutar con éxito, deberías ver en consola:
/*
[Assets] current_path=C:\Users\PTRUJILLO\Desktop\Trujillo\ProyectosCryxs\DofusLike\build\Debug
[Display] Viewport created: window=1280x720, virtual=1280x720, scale=1, viewport=(0,0,1,1)
[Display] View applied: center=(640,360), size=(1280x720)
[Pawn] Loading sprites...
[Assets] Loading texture: assets/sprites/player.png ... OK (found at: C:\...\build\Debug\assets\sprites\player.png, size=720x330)
[Pawn] sprite ON size=90x110 scale=0.837273 anim=8x90x110
[Entity] Loading sprites for player...
[Assets] Loading texture: assets/sprites/player.png ... OK (found at: C:\...\build\Debug\assets\sprites\player.png, size=720x330)
[Entity] Main texture: LOADED
[Entity] sprite ON size=90x110 scale=0.837273 anim=8x90x110
*/

// Si ves esto → TODO FUNCIONA CORRECTAMENTE ✅


// Si ves FAIL:
/*
[Assets] Loading texture: assets/sprites/player.png ... FAIL (searched from C:\...\build\Debug and parent directories)
[Pawn] sprite OFF (fallback)
*/

// Entonces:
// 1. Verifica que build/Debug/assets/ existe
// 2. Recompila con cmake --build . --config Debug (copia assets automáticamente)
// 3. O usa Assets::setRoot() con ruta absoluta
// 4. O activa FORCE_DEBUG_CIRCLE para al menos ver círculos


// ============================================================================
// MODO DEBUG DE CÍRCULOS - QUÉ ESPERAR
// ============================================================================

// Con FORCE_DEBUG_CIRCLE = true:
// - Pawn: círculo azul sólido, radio 16px, borde blanco
// - Entity (Player): círculo azul sólido, radio 16px, borde blanco
// - Entity (Enemy): círculo rojo sólido, radio 16px, borde blanco

// Logs esperados:
/*
[Pawn] FORCE_DEBUG_CIRCLE=true, usando círculo de radio 16px (sin sprites)
[Entity] FORCE_DEBUG_CIRCLE=true, usando círculo de radio 16px (sin sprites)
*/


// ============================================================================
// RECOMENDACIONES FINALES
// ============================================================================

/*
RECOMENDACIÓN 1: Usa la copia automática de assets (ya está ON por defecto)
  → No requiere cambios de código
  → Funciona desde cualquier working directory
  → Solo recompila: cmake --build . --config Debug

RECOMENDACIÓN 2: Si sigues teniendo problemas, usa FORCE_DEBUG_CIRCLE
  → Confirma que el problema es de assets, no de render
  → Entity::FORCE_DEBUG_CIRCLE = true;
  → Pawn::FORCE_DEBUG_CIRCLE = true;

RECOMENDACIÓN 3: Revisa los logs en consola
  → [Assets] debe mostrar OK para cada textura
  → [Display] debe mostrar viewport válido
  → [Pawn]/[Entity] debe mostrar "sprite ON"

RECOMENDACIÓN 4: Si ejecutas desde Visual Studio, asegúrate de que:
  → Working Directory = $(TargetDir) o $(ProjectDir)
  → O configura ASSET_ROOT en Environment
  → O usa Assets::setRoot() en el código
*/

