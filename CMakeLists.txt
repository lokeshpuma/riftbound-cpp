cmake_minimum_required(VERSION 3.20)
project(riftbound VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for language servers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTING "Build the testing suite" ON)

# Find or fetch raylib
if(EMSCRIPTEN)
    set(PLATFORM "Web" CACHE STRING "Target platform")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    # Use FetchContent for raylib when targeting WebAssembly
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(raylib)
else()
    find_package(raylib CONFIG QUIET)
    if(NOT raylib_FOUND)
        message(STATUS "raylib not found locally, fetching via FetchContent...")
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
        include(FetchContent)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.5
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(raylib)
    else()
        message(STATUS "Found raylib on system")
    endif()
endif()

# Common include directories
set(GAME_INCLUDES ${CMAKE_SOURCE_DIR}/include)

# Common game sources (excluding main.cpp for test reusability)
set(GAME_SOURCES
    src/core/Config.cpp
    src/core/AssetManager.cpp
    src/core/InputManager.cpp
    src/core/SaveSystem.cpp
    src/core/AudioPlayer.cpp
    src/core/ParticleSystem.cpp
    src/physics/AABB.cpp
    src/physics/PhysicsWorld.cpp
    src/combat/DamageInfo.cpp
    src/combat/Weapon.cpp
    src/combat/EmberBlade.cpp
    src/combat/PulseBolts.cpp
    src/combat/Projectile.cpp
    src/entities/Entity.cpp
    src/entities/Player.cpp
    src/entities/Enemy.cpp
    src/entities/Patroller.cpp
    src/entities/RangedSentry.cpp
    src/entities/Chaser.cpp
    src/entities/Boss.cpp
    src/level/Tile.cpp
    src/level/Level.cpp
    src/level/LevelManager.cpp
    src/level/CameraController.cpp
    src/level/Collectible.cpp
    src/level/Hazard.cpp
    src/level/Checkpoint.cpp
    src/ui/HUD.cpp
    src/ui/Menu.cpp
    src/ui/UIElements.cpp
    src/game/Game.cpp
)

# Static library for core gameplay logic
add_library(riftbound_core STATIC ${GAME_SOURCES})
target_include_directories(riftbound_core PUBLIC ${GAME_INCLUDES})
target_link_libraries(riftbound_core PUBLIC raylib)
if(NOT EMSCRIPTEN AND UNIX AND NOT APPLE)
    target_link_libraries(riftbound_core PUBLIC m pthread dl)
endif()

target_compile_features(riftbound_core PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(riftbound_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(riftbound_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Main executable target
add_executable(riftbound src/main.cpp)
target_link_libraries(riftbound PRIVATE riftbound_core)

# Target-specific properties for Web / Native
if(EMSCRIPTEN)
    set_target_properties(riftbound PROPERTIES
        OUTPUT_NAME "index"
        SUFFIX ".html"
    )
    target_link_options(riftbound PRIVATE
        "-sUSE_GLFW=3"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFORCE_FILESYSTEM=1"
        "--shell-file" "${CMAKE_SOURCE_DIR}/web/shell.html"
    )
    if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
        target_link_options(riftbound PRIVATE
            "--preload-file" "${CMAKE_SOURCE_DIR}/assets@assets"
        )
    endif()
else()
    # Native executable properties
    set_target_properties(riftbound PROPERTIES
        OUTPUT_NAME "riftbound"
    )
endif()

# Unit tests
if(BUILD_TESTING AND NOT EMSCRIPTEN)
    enable_testing()
    add_executable(riftbound_tests
        tests/test_main.cpp
        tests/test_physics.cpp
        tests/test_combat.cpp
        tests/test_progression.cpp
    )
    target_link_libraries(riftbound_tests PRIVATE riftbound_core)
    add_test(NAME AllTests COMMAND $<TARGET_FILE:riftbound_tests>)
endif()
