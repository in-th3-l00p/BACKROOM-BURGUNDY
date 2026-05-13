# escape — technical documentation

A Wolfenstein-style raycaster written in C++23 on top of SDL3, built with CMake. The render loop draws into a software framebuffer that is then uploaded to a streaming SDL texture.

## Module map

| Namespace | Location | Responsibility |
|-----------|----------|----------------|
| `escape::app` | `src/app/` | SDL window, renderer, software framebuffer, top-level `GameEngine` |
| `escape::game` | `src/game/` | Math (`Vector2`, `Size2D`), `Map`, `Tile` hierarchy, `Player`, `Raycaster`, `Sprite` + `SpriteRenderer`, `Texture` |
| `escape::ecs` | `src/ecs/` | Templated entity-component registry (`Registry`, `ComponentStorage<T>`) |
| `escape::errors` | `src/errors/` | Exception hierarchy rooted at `EscapeError : std::exception` |
| `escape::patterns` | `src/patterns/` | GoF patterns: `TileFactory` (Factory), `WallShadingStrategy` (Strategy) |
| `escape::templates` | `src/templates/` | Generic utilities (`Bounded<T>`, `clamp_into<T>`) |

## Frame pipeline

`GameEngine::run()` (`src/app/GameEngine.cpp:114`) is a fixed loop:

1. `Window::process_events` — drains SDL events, flips `is_open_` on quit/close.
2. `handle_input(dt)` — polls `SDL_GetKeyboardState`, calls `Player::try_move_*` and `Player::rotate_*`. Space-edge-detect toggles the nearest `DoorTile`.
3. `advance_doors(dt)` — iterates every cell, `dynamic_cast`s to `DoorTile*` and ticks its slide animation.
4. `render()` — three passes into `Framebuffer`:
   - `Raycaster::render_floor_and_ceiling` per-row floor casting (Lode V part 2).
   - `Raycaster::render_walls` per-column DDA wall casting, see below.
   - `SpriteRenderer::render` depth-sorted billboard sprites against the raycaster's depth buffer.
5. `Window::present_framebuffer` uploads the RGBA buffer to an `SDL_Texture` and presents it.

## Raycaster details

`src/game/Raycaster.cpp` implements all three parts of Lode V's tutorial.

**Part 1 — DDA wall casting.** `make_dda_state` sets up `delta_dist_*`, `side_dist_*`, and `step_*`. `cast_ray` steps until a non-zero cell is hit, then computes `perp_distance` and `wall_x`.

**Part 2 — Texturing, floor/ceiling.** Wall pixels sample the hit `Tile` via virtual `Tile::sample(wall_u, wall_v, side)` (so the raycaster never knows whether the tile is `SolidWallTile`, `TexturedWallTile`, or `EmissiveWallTile`). Floor casting per Lode V: for each scanline below the horizon, compute a row distance, march `(floor_x, floor_y)` by a constant step per column, sample `floor_texture_` and the ceiling-mirror pixel.

**Part 3 — Transparency.** `cast_ray_multi` keeps DDA going as long as the current tile's `passes_ray_through()` returns true (`ThinWallTile` always, `DoorTile` once it has begun opening). Hits are rendered **back-to-front**, so a fence overlays the solid wall behind it through its alpha-zero pixels. `depth_buffer_[x]` stores the *last* (solid) hit's distance so sprites are correctly visible behind transparent tiles.

## Map and tiles

`Map` (`src/game/Map.cpp`) owns `std::vector<std::unique_ptr<Tile>>`. Copy is a deep clone via `Tile::clone()`; assignment is copy-and-swap. `Map::textured_tile_count` uses `dynamic_cast` to count `TexturedWallTile`s.

`Tile` is the abstract base; the polymorphic interface is `clone`, `is_passable`, `sample`, `passes_ray_through`, plus `do_print` (NVI display). Derivatives: `EmptyTile`, `SolidWallTile`, `TexturedWallTile`, `EmissiveWallTile` (4th class, integrated via `Map::demo_tile_for_cell`), plus `DoorTile` and `ThinWallTile` for Lode V part 3.

`TileFactory::with_demo_registry` registers a lambda builder for each cell id; `GameEngine` uses it to drop doors/fences into the demo map after `Map::build_demo_map`.

## Shading strategy

`Raycaster::shading_` holds a `std::unique_ptr<WallShadingStrategy>`. `GameEngine` installs `FogShading` at startup; `LinearDistanceShading` and `NoShading` are alternatives. The raycaster invokes `shading_->apply(color, distance)` on every wall pixel.

## Templates

`Bounded<T>` (`src/templates/Bounded.hpp`) holds a value clamped to `[min, max]`. `GameEngine` uses `Bounded<float> fps_target_` and `Bounded<int> player_inventory_`. The companion `clamp_into<T>` template function is instantiated for both `<float>` and `<int>` in `main.cpp`.

`Registry::emplace/get/has/each` (`src/ecs/Registry.hpp`) and `ComponentStorage<T>` are templated and instantiated with `Player` and other component types.

## Exceptions

`EscapeError : std::exception` is the base. `MapLoadError`, `TextureLoadError`, `OutOfBoundsError`, `InvalidConfigError` are thrown from constructors (e.g., `Map`, `Texture`, `DoorTile`) and caught in `main` via two layered handlers (`EscapeError&` then `std::exception&`).

## Controls

- `W/S` forward/backward
- `A/D` strafe
- `LEFT/RIGHT` rotate (yaw)
- `SPACE` toggle the nearest door in front

## Build

```sh
brew install sdl3
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/oop
```
