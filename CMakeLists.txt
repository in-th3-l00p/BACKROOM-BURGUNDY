cmake_minimum_required(VERSION 3.26)

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing executable name in this file
# for now, the project name is used as the executable name
set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")


project(${MAIN_PROJECT_NAME})
# set(CMAKE_PROJECT_VERSION_MAJOR 0)
# set(CMAKE_PROJECT_VERSION_MINOR 0)
# set(CMAKE_PROJECT_VERSION_PATCH 1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

###############################################################################

# external dependencies with FetchContent
# include(FetchContent)
#
# set(FETCHCONTENT_QUIET OFF CACHE INTERNAL "Override verbose output" FORCE)
# set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE INTERNAL "Override update strategy" FORCE)
#
# NOTE: Also update env vars used for caching in
#     - .github/actions/configure-cmake/action.yml
#     - .github/workflows/cmake.yml
# FetchContent_Declare(
#     SomeLib
#     GIT_REPOSITORY https://github.com/<SomeUser>/<SomeLib>.git
#     GIT_TAG        <some_git_hash> # <which tag/branch @ <some_date>>
#     GIT_SHALLOW    1  # works only with branches or tags, not with arbitrary commit hashes
#     GIT_PROGRESS   ON
# )
#
# FetchContent_MakeAvailable(SomeLib)

###############################################################################

# external dependencies with find_package

# find_package(Threads REQUIRED)

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
    message(STATUS "SDL3 not found via find_package, fetching it from source")
    include(FetchContent)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL_INSTALL_TESTS OFF CACHE BOOL "" FORCE)
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

include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.5
    GIT_SHALLOW 1
    GIT_PROGRESS ON
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(imgui SYSTEM PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC SDL3::SDL3)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW 1
)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(nlohmann_json)

###############################################################################

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing name here
add_executable(${MAIN_EXECUTABLE_NAME}
    main.cpp
)
target_sources(${MAIN_EXECUTABLE_NAME}
    PRIVATE
        src/app/GameEngine.cpp
        src/app/Window.cpp
        src/app/Framebuffer.cpp
        src/app/ImGuiOverlay.cpp
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
        src/burgundy/Item.cpp
        src/burgundy/Inventory.cpp
        src/burgundy/Puzzle.cpp
        src/burgundy/KeypadPuzzle.cpp
        src/burgundy/CombinationPuzzle.cpp
        src/burgundy/ExaminePuzzle.cpp
        src/burgundy/Interactable.cpp
        src/burgundy/PickupInteractable.cpp
        src/burgundy/PuzzleInteractable.cpp
        src/burgundy/ExitInteractable.cpp
        src/burgundy/GameDirector.cpp
        src/burgundy/SceneLoader.cpp
)

# NOTE: Add all defined targets (e.g. executables, libraries, etc. )
# NOTE: RUN_SANITIZERS is optional, if it's not present it will default to true
set_compiler_flags(RUN_SANITIZERS ${USE_ASAN} TARGET_NAMES ${MAIN_EXECUTABLE_NAME})
# set_compiler_flags(TARGET_NAMES ${MAIN_EXECUTABLE_NAME} ${FOO} ${BAR})
# where ${FOO} and ${BAR} represent additional executables or libraries
# you want to compile with the set compiler flags

###############################################################################

# use SYSTEM so cppcheck and clang-tidy do not report warnings from these directories
target_include_directories(${MAIN_EXECUTABLE_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)
target_include_directories(${MAIN_EXECUTABLE_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE
    ext
)
# target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE ${<SomeLib>_SOURCE_DIR}/include)
# target_link_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${<SomeLib>_BINARY_DIR}/lib)
target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE SDL3::SDL3 imgui nlohmann_json::nlohmann_json)

# treat SDL3 headers as system to keep -Werror clean
if(TARGET SDL3::SDL3)
    get_target_property(_sdl3_inc SDL3::SDL3 INTERFACE_INCLUDE_DIRECTORIES)
    if(_sdl3_inc)
        target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE ${_sdl3_inc})
    endif()
endif()

###############################################################################

# copy binaries to "bin" folder; these are uploaded as artifacts on each release
# DESTINATION_DIR is set as "bin" in cmake/Options.cmake:6
install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(FILES tastatura.txt DIRECTORY assets COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
# copy_files(FILES tastatura.txt config.json DIRECTORY images sounds COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
# copy_files(DIRECTORY images sounds COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
