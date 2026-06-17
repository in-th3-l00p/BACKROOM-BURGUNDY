# backrooms

A software raycaster written in modern C++23 on top of [SDL3](https://www.libsdl.org/).
It implements a Wolfenstein 3D–style renderer following Lode Vandevenne's classic
[raycasting tutorial](https://lodev.org/cgtutor/raycasting.html) (parts 1–3): textured
walls via DDA, procedural floor/ceiling casting, depth-sorted billboard sprites, sliding
doors and transparent thin walls.

## Project layout

```
backrooms/
├── CMakeLists.txt        # single, self-contained build
├── src/
│   ├── main.cpp          # entry point / demo scenario
│   ├── app/              # SDL window, framebuffer, GameEngine
│   ├── game/             # math, Map, Tile hierarchy, Player, Raycaster, sprites
│   ├── ecs/              # small templated entity-component registry
│   ├── errors/           # exception hierarchy (EscapeError : std::exception)
│   ├── patterns/         # TileFactory (Factory), WallShadingStrategy (Strategy)
│   └── templates/        # generic helpers (Bounded<T>, clamp_into<T>)
├── docs/architecture.md  # technical walkthrough
└── assets/               # runtime assets
```

Each module folder keeps headers next to their `.cpp` files. The C++ code lives in the
`escape::` namespace. See [`docs/architecture.md`](docs/architecture.md) for a full
walkthrough of the frame pipeline and raycaster internals.

## Build

Requires a C++23 compiler and CMake ≥ 3.26. SDL3 is found automatically if installed,
otherwise it is fetched and built from source.

```sh
brew install sdl3                                  # optional; otherwise fetched from source
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

Optional AddressSanitizer/UBSan build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON
cmake --build build --parallel
```

The project opens cleanly in CLion (open the folder; CMake is detected automatically).

## Run

```sh
./build/backrooms
```

Set `ESCAPE_HEADLESS=1` to run the construction/demo scenario and exit without opening a
window (useful for quick smoke tests):

```sh
ESCAPE_HEADLESS=1 ./build/backrooms
```

## Controls

| Key            | Action                          |
|----------------|---------------------------------|
| `W` / `S`      | move forward / backward         |
| `A` / `D`      | strafe left / right             |
| `←` / `→`      | rotate (yaw)                    |
| `Space`        | open/close the door in front    |

## Documentation

- [`docs/game-design.md`](docs/game-design.md) — the game design (story, mechanics, content)
- [`docs/architecture.md`](docs/architecture.md) — technical walkthrough of the engine
- [`docs/project-management.md`](docs/project-management.md) — phases, gates, conventions, status
- [`docs/subagents.md`](docs/subagents.md) — development-agent registry

## License

Licensed under [AGPLv3](LICENSE).
