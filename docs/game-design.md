# Game design — Backrooms Burgundy

> **Status: in discussion.** Items marked **🟡 PROPOSAL** are my suggestions, not
> decisions — we lock them through conversation. Items marked **✅ DECIDED** are agreed.
> Open questions are listed at the bottom.

## 1. Concept

**🟡 PROPOSAL** A first-person liminal-space horror explorer. You are lost in an endless,
slightly-wrong interior — burgundy-papered corridors, drop ceilings, damp carpet, buzzing
fluorescent lights — and you must find the way out before *something* finds you.

- **Genre:** atmospheric horror / escape exploration (no traditional combat).
- **Engine reality:** software raycaster (Wolfenstein-style), so the world is a grid of
  walls/doors/sprites. Design must stay within that.

## 2. Pillars

**🟡 PROPOSAL**
1. **Dread over action** — tension, not shooting.
2. **Getting lost** — samey, repeating spaces; navigation *is* the challenge.
3. **The burgundy mood** — a committed, oppressive color identity.

## 3. Story / setting

**🔲 TO DISCUSS** — who is the player, why are they here, is there narrative framing
(notes, intercom, none / pure environmental), what is the "exit"?

## 4. Core loop

**🔲 TO DISCUSS** — the moment-to-moment. Candidate: *explore → find keys/clues → open the
way → reach the exit, while avoiding the entity.* What's the win condition? How long is a run?

## 5. Mechanics

- **Movement / interaction:** WASD + rotate + Space (open doors) — already implemented. ✅
- **Objective items:** **🔲 TO DISCUSS** (keys? fuses? notes? none?)
- **Player resources:** **🔲 TO DISCUSS** (health? sanity? flashlight battery? a timer? nothing — pure stealth?)
- **Inventory:** the existing door-counter becomes a real inventory once we know what's collected. **🟡 PROPOSAL**

## 6. The threat (entity)

**🔲 TO DISCUSS** — is there an entity at all? If so:
- One or many? Always present or triggered (by sound/sight)?
- Behavior: wander → chase on line-of-sight? sound-driven? patrol routes?
- Consequence of contact: instant death / damage / sanity drain / reset to checkpoint?
- Rendered as a billboard sprite via the existing depth-buffered `SpriteRenderer`. **🟡 PROPOSAL**

## 7. Levels & progression

**🔲 TO DISCUSS** — hand-authored levels loaded from files (`assets/levels/*.txt` via
`Map::load_from_file`) vs procedural generation; how many; linear vs hub; difficulty curve.

## 8. Art direction

- **Palette:** burgundy / deep maroon walls, dim warm-to-sickly lighting, dark fog. **🟡 PROPOSAL**
- **Textures (procedural PPM):** burgundy paneling, damp/stained wall, patterned carpet,
  drop-ceiling, fluorescent light panel. **🟡 PROPOSAL** (first set)
- **Lighting:** burgundy-tuned `FogShading` for depth; flickering `EmissiveWallTile` lights. **🟡 PROPOSAL**

## 9. Audio

**🔲 TO DISCUSS** — procedural drone / hum, footsteps, entity cue, stingers? (SDL3 audio.)

## 10. UI / HUD

**🔲 TO DISCUSS** — minimal HUD (inventory, objective?), title screen, death/win screens.

## 11. Technical mapping (how design maps to the engine)

| Design element | Engine mechanism |
|----------------|------------------|
| Walls / rooms | `Tile` hierarchy + `Map` grid |
| Doors / keys | `DoorTile` (+ key gating, TBD) |
| Lights | `EmissiveWallTile` + flicker |
| Entity, items, props | `Sprite` + `SpriteRenderer` (depth-sorted billboards) |
| Mood / distance | `WallShadingStrategy` (`FogShading`) |
| Levels | `Map::load_from_file` + `assets/levels/*.txt` |
| Game state | new `GameState` (Playing/Won/Lost) in `app/GameEngine` |

## Open questions (the agenda for our discussion)

1. **Win condition & run length** — what does "escape" concretely mean, and how long is a session?
2. **The entity** — is there one, and how lethal / how does it behave?
3. **Player resources & fail state** — health/sanity/battery/timer, or pure stealth?
4. **Objective items** — keys / fuses / notes / none?
5. **Story framing** — narrative or pure environmental?
6. **Levels** — hand-authored vs procedural; how many.
