# Subagents — development agent registry

How subagents are used to build Backrooms Burgundy, and a running log of the ones
actually spawned. Orchestration mode: **lightweight** (delegate per task, human in the
loop between phases). These are *development* agents — not in-game AI entities (the
in-game entity is described in [`game-design.md`](game-design.md)).

## Roles

| Role | Agent type | When to use | Returns |
|------|-----------|-------------|---------|
| **Scout** | `Explore` (read-only) | "How does X work / where is Y" sweeps across files | A conclusion, not file dumps |
| **Architect** | `Plan` | Design-heavy decisions; optionally several proposals scored against each other | A step-by-step plan / chosen approach |
| **Implementer** | `fork` (+ `isolation: worktree` when editing in parallel) | Building an independent feature slice | Code changes (merged after gate) |
| **Reviewer** | `fork` / `general-purpose` | Adversarial review of a diff before merge | Findings; only real ones block |

The hard gate (build + headless smoke run) backs every implementer's work — see
[`project-management.md`](project-management.md#conventions).

## Feature slices → modules

Independent slices that parallelize cleanly:

| Slice | Module |
|-------|--------|
| Level loading from files | `game/Map` |
| Objective + win/lose state machine | `app/GameEngine` + new `game/` types |
| Entity / pursuer (AI sprite) | `game/Sprite` + `ecs/` |
| Atmosphere (shading, textures) | `patterns/WallShadingStrategy`, `game/Texture` |
| Asset pipeline | `tools/` + `assets/` |
| HUD / menus | `app/` |

## Agent log

A row per spawned agent, appended as we go.

| Date | Role | Agent type | Task | Phase | Outcome |
|------|------|-----------|------|-------|---------|
| _(none yet)_ | | | | | |
