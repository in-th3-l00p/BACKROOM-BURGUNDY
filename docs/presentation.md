# Presentation — homework requirements map

Each requirement points to the file(s) and tag where it is satisfied.

## Tema 0

| Requirement | Where |
|-------------|-------|
| Nume proiect | `escape` — declared in `CMakeLists.txt:5` and `README.md` |
| Descriere temă | `README.md` opening section + `docs/architecture.md` |

## Tema 1 — tag `v0.1`

| Requirement | Where |
|-------------|-------|
| ≥3-4 clase prin compunere | `Vector2`, `Size2D`, `RenderStyle`, `Map`, `Player`, `Raycaster` in `src/game/` |
| Constructori cu parametri pentru fiecare | e.g. `Player(Vector2, Vector2, Vector2, float, float)` în `src/game/Player.hpp:11` |
| O singură clasă cu copy ctor + copy `=` + dtor | `Player` în `src/game/Player.cpp:18-30` |
| `operator<<` pentru toate clasele cu compunere | toate fișierele din `src/game/`, `src/app/Window.cpp`, `src/app/Framebuffer.cpp` |
| Multe `const` și funcții `private` | toate header-ele; e.g. `Raycaster::pick_wall_color` privat în `src/game/Raycaster.hpp` |
| ≥3 funcții publice netriviale | `Player::try_move_forward/rotate_left` `src/game/Player.cpp`, `Map::load_from_file` `src/game/Map.cpp:177`, `Raycaster::cast_ray` `src/game/Raycaster.cpp:88` |
| Scenariu de utilizare în `main` | `main.cpp` construiește toate obiectele și pornește engine-ul |
| `tastatura.txt` cu exemple intrare | `tastatura.txt` (W/A/S/D) |
| ≥52-60% C++ + `.gitattributes` | `.gitattributes` setează `*.cpp`/`*.hpp`/`*.h` ca `linguist-language=C++` |
| Tag `v0.1` | `git tag v0.1` pe commit `15599cc` |
| CI cu toate bifele | `.github/workflows/cmake.yml` (template moștenit) |

## Tema 2 — tag `v0.2`

| Requirement | Where |
|-------------|-------|
| Separare `.hpp`/`.cpp` | toate clasele în `src/` |
| 1 bază + ≥3 derivate din aceeași ierarhie | `Tile` (`src/game/Tile.hpp`) cu `EmptyTile`, `SolidWallTile`, `TexturedWallTile` |
| Funcție virtuală specifică temei | `Tile::sample(wall_u, wall_v, side)` — pereți texturați/colorați/pulsanți |
| Constructor virtual (clone) | `Tile::clone()` peste tot în derivate |
| Afișare virtuală cu interfață non-virtuală | `Tile::print` (NVI) apelează `do_print` în `src/game/Tile.cpp:7` |
| Apel constructor bază din derivate | implicit (`Tile() = default`) folosit de orice `XxxTile(...)` |
| Clasă cu atribut pointer la bază + derivate | `Map` ține `std::vector<std::unique_ptr<Tile>>` în `src/game/Map.hpp:48` |
| Suprascris cc/op= cu copy-and-swap | `Map(const Map&)` clonează, `operator=(Map)` + `swap` în `src/game/Map.cpp:65-92` |
| `dynamic_cast` cu sens | `Map::textured_tile_count` `src/game/Map.cpp:147`, `GameEngine::advance_doors` și `find_door_in_front` în `src/app/GameEngine.cpp` |
| Ierarhie excepții cu bază `std::exception` | `src/errors/Exceptions.hpp` — `EscapeError`, `MapLoadError`, `TextureLoadError`, `OutOfBoundsError`, `InvalidConfigError` |
| Throw în constructori + try/catch în `main` | `Map`, `Texture`, `DoorTile`, `ThinWallTile`, `Sprite` constructori throw; `main.cpp` catch pe `EscapeError&` apoi `std::exception&` |
| Funcții/atribute `static` | `Texture::invalid_count_` + `Texture::invalid_texture_count` în `src/game/Texture.cpp:11/116`, `Map::build_demo_map`/`Map::load_from_file`/`Map::demo_tile_for_cell` static, `TileFactory::with_demo_registry` static |
| STL | `std::vector`, `std::unique_ptr`, `std::shared_ptr`, `std::unordered_map`, `std::sort`, lambdas peste tot |
| Funcții *high-level* (fără getters/setters inutili) | `Player::try_move_forward`, `Raycaster::render`, `DoorTile::toggle/advance` |
| ≥75-78% C++ | `.gitattributes` |
| Commit final cu a 4-a derivată integrată | `EmissiveWallTile` în commit `2579c1f` „added the emissive wall tile as a fourth derived class”, integrat via `Map::demo_tile_for_cell` |
| Tag `v0.2` | `git tag v0.2` pe commit-ul de mai sus |

## Tema 3 — tag `v0.3`

| Requirement | Where |
|-------------|-------|
| 2 design patterns | **Factory**: `patterns::TileFactory` în `src/patterns/TileFactory.hpp` + `with_demo_registry`. **Strategy**: `patterns::WallShadingStrategy` în `src/patterns/WallShadingStrategy.hpp` cu `NoShading`/`LinearDistanceShading`/`FogShading`, folosită în `Raycaster::shading_` |
| Clasă template cu ≥2 instanțieri | `templates::Bounded<T>` în `src/templates/Bounded.hpp` — instanțiată `Bounded<float> fps_target_` și `Bounded<int> player_inventory_` în `src/app/GameEngine.hpp` |
| Funcție template cu ≥2 instanțieri (opțional) | `templates::clamp_into<T>` în `src/templates/Bounded.hpp`, instanțiată `<float>` și `<int>` în `main.cpp` |
| ≥80-90% C++ | `.gitattributes` |
| Tag `v0.3` | `git tag v0.3` pe commit `3bd0521` |

## Lode V raycaster — coverage

| Tutorial | Implementation |
|----------|----------------|
| [Part 1](https://lodev.org/cgtutor/raycasting.html) — untextured walls, DDA, player rotation/strafe | `Raycaster::cast_ray` + `Player::rotate_*`/`try_move_*` |
| [Part 2](https://lodev.org/cgtutor/raycasting2.html) — texturing, floor/ceiling casting, sprites | `TexturedWallTile`, `Raycaster::render_floor_and_ceiling`, `SpriteRenderer` + depth buffer |
| [Part 3](https://lodev.org/cgtutor/raycasting3.html) — doors, transparency, animated walls | `DoorTile` (slide + toggle), `ThinWallTile` (alpha texture), `Raycaster::cast_ray_multi` for back-to-front transparent layers, `EmissiveWallTile` (animated pulse) |

## Branches & worktree history

- `main` — current shipped state.
- `fix/doors-transparency` — preserved branch where multi-hit transparency was developed in a worktree, merged back via `git merge --no-ff` (commit `e3b40db`).
