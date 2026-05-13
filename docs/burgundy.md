# BACKROOM BURGUNDY — game design and AI expansion

## Concept

You wake up in the back office of *The Burgundy Room*, a defunct 1970s soul lounge. Velvet wallpaper, brass fixtures gone green, a jukebox humming a warped loop. The owner vanished in '74 with the safe combination. You have until sunrise to find your way out.

Built on the project's raycaster — Wolfenstein-style 90° cells are a natural fit for cramped lounges, hallways, kitchen, stage, and vault.

## Aesthetic

| Element | Where |
|---------|-------|
| Burgundy damask wallpaper | `TileFactory::damask_texture` (`src/patterns/TileFactory.cpp`) |
| Velvet brickwork | `velvet_brick_texture` |
| Brass paneling with patina | `brass_panel_texture` |
| Kitchen tile | `kitchen_tile_texture` |
| Bead-curtain dividers | `bead_curtain_texture` (alpha texture, ray-passes-through) |
| Vault door | `vault_door_texture` |
| Jukebox glow | `EmissiveWallTile` cell id `5` |

Floor: smoke-dark warm checker. Ceiling: nicotine gradient. Fog shading by distance via `FogShading` strategy.

## Controls

- `W/A/S/D` move/strafe
- `←/→` look
- `E` interact with whatever is in front of you
- `Space` toggle a door
- ImGui windows: Burgundy Room HUD, Inventory, Narrative log, Help

## Puzzles

Each puzzle is a class deriving from `burgundy::Puzzle` and rendered through `Puzzle::draw_ui()`:

- `KeypadPuzzle` — digit string entry (the jukebox: matchbook years 66/67/72/73 → `66677273`).
- `CombinationPuzzle` — n-wheel dial (the safe behind the portrait: `12-24-08-30`).
- `ExaminePuzzle` — paged inspection that unlocks revelations (the liquor cabinet → framed photo → number etched on brass).

The fourth content piece is the **back door** `ExitInteractable`, gated on `vault.is_solved()`.

## Data-driven scene file

Content lives in `assets/burgundy/scene.json` and is parsed by `burgundy::SceneLoader`. Schema:

```jsonc
{
  "title": "...",
  "intro": "...",
  "spawn":   { "x": 3.5, "y": 3.5, "dir_x": 1.0, "dir_y": 0.0 },
  "palette": { ".": 0, "#": 1, "W": 2, "V": 3, "K": 4, "L": 5, "D": 6, "C": 7 },
  "layout":  [ "########################", "#......#KKKKK#...", ... ],
  "items": [
    { "id": "matchbook_blue", "title": "...", "description": "..." }
  ],
  "puzzles": [
    { "id": "jukebox",    "kind": "keypad",      "title": "...", "prompt": "...", "solution": "66677273", "reward_item": "cabinet_key", "reward_message": "..." },
    { "id": "vault",      "kind": "combination", "solution": [12, 24, 8, 30], "wheel_max": 39, ... },
    { "id": "cabinet",    "kind": "examine",     "revelations": ["...", "..."] }
  ],
  "interactables": [
    { "x": 4, "y": 2, "kind": "pickup", "item": "matchbook_blue", "label": "...", "narrative": "..." },
    { "x": 13, "y": 12, "kind": "puzzle", "puzzle": "jukebox", "label": "Jukebox", "narrative": "..." },
    { "x": 23, "y": 17, "kind": "exit",  "label": "Back door", "requires_puzzle": "vault", "narrative": "..." }
  ],
  "opening_narrative": ["...", "..."]
}
```

Palette glyphs map characters in `layout` to `TileFactory::with_burgundy_registry()` cell ids. Anything not in the palette is treated as floor.

## AI expansion model

The engine knows nothing about story content — it only knows how to load JSON. To author a new variant of the game (or a new room), an LLM generates a JSON file that conforms to the schema above and drops it into `assets/burgundy/`. Launch with a custom path:

```cpp
escape::app::GameEngine engine {
    escape::app::GameEngineConfig {
        escape::app::WindowConfig {"BACKROOM BURGUNDY", 1280, 720},
        "assets/burgundy/my-generated-scene.json",
    }
};
```

Or pass it on the command line if you wire one in `main.cpp`.

### What an LLM is responsible for

1. **Layout** — an ASCII map using palette glyphs, with walls forming closed rooms and `.` for traversable floor.
2. **Items** — short title + flavour description; each `id` must be unique.
3. **Puzzles** — pick from the three kinds (`keypad`, `combination`, `examine`), supply a `solution`, and an optional `reward_item` / `reward_message`. Add hints to the matching `Item` descriptions so the puzzle is solvable from clues in the world.
4. **Interactables** — place tiles for each pickup/puzzle/exit, with `label` and `narrative` that fits the noir mood.
5. **Opening narrative** — three or four lines that orient the player.

### What stays in C++

Any new *kind* of puzzle (e.g. a wire-cutting minigame) needs a new `Puzzle` subclass with a `draw_ui()` implementation and a registration in `SceneLoader::make_puzzle`. The same is true for new `Interactable` subtypes. This is the only place the AI's reach stops; everything else is data.

### Suggested generation prompt scaffold

```
You are authoring a JSON scene file for a Wolfenstein-style escape room game called BACKROOM BURGUNDY.
The mood is 1970s soul lounge noir. Output ONLY valid JSON that matches this schema: <paste schema>.
Constraints: map must be 24×24, perimeter sealed, palette uses {., #, W, V, K, L, D, C}, exactly one
exit interactable that requires a puzzle, every puzzle reward must be referenced by an exit or another
puzzle's hints, every clue needed to solve a puzzle must appear in an item description in the same scene.
```

### Validation

`SceneLoader` throws `MapLoadError` / `InvalidConfigError` (both inherit `EscapeError`) on bad input. `main.cpp` catches both layers, so a malformed AI-generated scene fails cleanly with a message rather than crashing.

## Module references

| Concept | Header |
|---------|--------|
| Game state hub | `src/burgundy/GameDirector.hpp` |
| Items / inventory | `src/burgundy/Item.hpp`, `src/burgundy/Inventory.hpp` |
| Puzzle base + 3 kinds | `src/burgundy/Puzzle.hpp`, `KeypadPuzzle.hpp`, `CombinationPuzzle.hpp`, `ExaminePuzzle.hpp` |
| World interactions | `src/burgundy/Interactable.hpp`, `PickupInteractable.hpp`, `PuzzleInteractable.hpp`, `ExitInteractable.hpp` |
| JSON loader | `src/burgundy/SceneLoader.hpp` |
| ImGui overlay | `src/app/ImGuiOverlay.hpp` |
| Burgundy textures + factory | `src/patterns/TileFactory.cpp` (`with_burgundy_registry`) |
| Engine wiring | `src/app/GameEngine.cpp` (`draw_overlay_ui`, `try_interact`) |
