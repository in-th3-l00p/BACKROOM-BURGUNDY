cmake_minimum_required(VERSION 3.26)
project(backrooms CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(USE_ASAN "Build with AddressSanitizer/UBSan" OFF)

# ---------------------------------------------------------------------------
# Dependencies: SDL3 (prefer an installed copy, otherwise fetch from source).
# ---------------------------------------------------------------------------
execute_process(
    COMMAND brew --prefix sdl3
    OUTPUT_VARIABLE SDL3_BREW_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(SDL3_BREW_PREFIX)
    list(PREPEND CMAKE_PREFIX_PATH "${SDL3_BREW_PREFIX}")
endif()

find_package(SDL3 CONFIG QUIET)
if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found, fetching it from source")
    include(FetchContent)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.22
        GIT_SHALLOW 1
        GIT_PROGRESS ON
    )
    FetchContent_MakeAvailable(SDL3)
endif()

# ---------------------------------------------------------------------------
# Target
# ---------------------------------------------------------------------------
add_executable(backrooms
    src/main.cpp
    src/app/GameEngine.cpp
    src/app/Window.cpp
    src/app/Framebuffer.cpp
    src/game/Vector2.cpp
    src/game/Size2D.cpp
    src/game/RenderStyle.cpp
    src/game/Map.cpp
    src/game/Player.cpp
    src/game/Raycaster.cpp
    src/game/Texture.cpp
    src/game/Tile.cpp
    src/game/EmptyTile.cpp
    src/game/SolidWallTile.cpp
    src/game/TexturedWallTile.cpp
    src/game/EmissiveWallTile.cpp
    src/game/Sprite.cpp
    src/game/SpriteRenderer.cpp
    src/game/DoorTile.cpp
    src/game/ThinWallTile.cpp
    src/patterns/TileFactory.cpp
    src/patterns/WallShadingStrategy.cpp
)

target_include_directories(backrooms PRIVATE src)

if(MSVC)
    target_compile_options(backrooms PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(backrooms PRIVATE -Wall -Wextra -pedantic)
endif()

if(USE_ASAN AND NOT MSVC)
    target_compile_options(backrooms PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(backrooms PRIVATE -fsanitize=address,undefined)
endif()

# Treat SDL3 headers as system headers so our -Wall/-Wextra stays clean.
if(TARGET SDL3::SDL3)
    get_target_property(_sdl3_inc SDL3::SDL3 INTERFACE_INCLUDE_DIRECTORIES)
    if(_sdl3_inc)
        target_include_directories(backrooms SYSTEM PRIVATE ${_sdl3_inc})
    endif()
endif()

target_link_libraries(backrooms PRIVATE SDL3::SDL3)

install(TARGETS backrooms RUNTIME DESTINATION bin)
