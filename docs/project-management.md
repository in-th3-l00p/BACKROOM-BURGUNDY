# Project management — Backrooms Burgundy

This is the living source of truth for *how* the project is run: phases, gates,
conventions, and current status. Game *content/design* lives in
[`game-design.md`](game-design.md); the development agents live in
[`subagents.md`](subagents.md).

## Conventions

- **Branch model:** work on a short-lived branch, fast-forward `main`, delete the branch.
- **Commits:** small, logical, each one builds. Imperative subject, body explains *why*.
- **Push:** after every commit (`git push origin main`). Always.
- **Gate (every phase must pass before it's done):**
  1. Clean build from scratch: `rm -rf build && cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build --parallel`
  2. Zero warnings under `-Wall -Wextra -pedantic`.
  3. Headless smoke run is green: `ESCAPE_HEADLESS=1 ./build/backrooms` → exit 0.
  4. (When behavior should be unchanged) byte-diff headless output against a saved baseline.

## Roadmap

| Phase | Goal | Task | Status |
|------:|------|------|--------|
| Docs  | Game design + PM + subagent docs (living, foundational) | #7 | 🔄 in progress |
| 0 | Foundations: asset generator + first textures, file-based levels, GameState scaffold | #8 | ⛔ blocked by docs |
| 1 | Core loop: exit/keys/doors, real inventory, lose condition | #9 | ⛔ blocked by docs |
| 2 | Atmosphere: burgundy textures, fog, flickering lights, audio drone | #10 | ⛔ blocked by docs |
| 3 | Threat: sprite-based pursuer with chase AI | #11 | ⛔ blocked by docs |
| 4 | Polish: HUD, title/death screens, more levels | #12 | ⛔ blocked by docs |

The documentation task gates the rest: we agree on the design before building it.

## Locked decisions

- **Project/executable name:** `backrooms` (C++ namespace remains `escape::`).
- **Layout:** headers beside sources under `src/<module>/`; `main.cpp` in `src/`.
- **Tooling:** stripped to a single self-contained `CMakeLists.txt` (no CI, no cmake/ modules).
- **Assets:** procedural → PPM (`tools/gen_assets.py`, zero deps; loaded via `Texture::load_from_ppm`).
- **Orchestration:** lightweight subagents (Explore / Plan / fork+worktree / review), check-in between phases.

## Status log

- 2026-06-17 — Restructured project to CLion-style layout; renamed to `backrooms`; stripped university tooling/CI. Verified (build + headless run byte-identical to baseline). Pushed.
- 2026-06-17 — Set up documentation + PM task structure (this doc, game-design, subagents).
