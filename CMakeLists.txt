cmake_minimum_required(VERSION 3.16)
project(DofusLikeProto CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(DofusLike
    src/main.cpp
    src/app/App.cpp
    src/map/Map.cpp
    src/map/Isometric.cpp
    src/units/Entity.cpp
    src/units/Pawn.cpp
    src/systems/TurnSystem.cpp
    src/systems/Pathfinding.cpp
    src/systems/LineOfSight.cpp
)

target_include_directories(DofusLike PRIVATE src)

target_link_libraries(DofusLike PRIVATE SFML::Graphics SFML::Window SFML::System)
