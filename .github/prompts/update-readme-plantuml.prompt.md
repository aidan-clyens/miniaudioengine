---
description: "Update the software architecture PlantUML component diagram in README.md to reflect the current state of the codebase. Discovers all classes, interfaces, and relationships by reading headers, then rewrites the diagram section."
agent: "agent"
---

Update the **Software Architecture** PlantUML component diagram in `README.md` so it accurately reflects the current codebase.

## Step 1 — Discover the codebase structure

Read headers from each layer to identify what exists:

| Layer | Path | What to look for |
|-------|------|-----------------|
| Framework (0) | `src/framework/include/interfaces/` | All `I*` interfaces |
| Framework (0) | `src/framework/include/` | PImpl handles (`Device`, `File`), utilities |
| Control (3) | `src/control/audio/include/`, `src/control/midi/include/` | Controller classes |
| Data (1) | `src/data/audio/include/`, `src/data/midi/include/` | DataPlane + CallbackHandler classes |
| Processing (2) | `src/processing/audio/include/`, `src/processing/sampleplayer/include/` | Processor interfaces and implementations |
| Public (4) | `include/miniaudioengine/` | Public API classes; note any `[[deprecated]]` type aliases |

For each class/interface found, record:
- Its **name** and **namespace**
- Its **base class** (inheritance)
- Its **key associations** (owned members, uses, calls)
- Whether it is **deprecated** (exclude deprecated stubs from the diagram)

## Step 2 — Identify what has changed vs the existing diagram

Read the current PlantUML block inside `## Software Architecture` in `README.md` and note every discrepancy:
- Classes that no longer exist or are now deprecated
- New classes or interfaces not yet shown
- Relationships that are wrong (wrong arrow type, wrong direction, missing label)
- Packages that are missing or mislabelled

## Step 3 — Rewrite the diagram

Replace **only** the PlantUML code block within `## Software Architecture` in `README.md`.

Follow these conventions:
- One `package` per layer, coloured consistently:
  - Framework (Layer 0): `#lightblue`
  - Data Plane (Layer 1): `#lightgreen`
  - Processing Plane (Layer 2): `#lightyellow`
  - Control Plane (Layer 3): `#lightsalmon`
  - Public / CLI / Examples (Layer 4): `#lightgray`
- Use `interface "IFoo"` for abstract/interface types; `[BarClass]` for concrete components.
- Arrow semantics:
  - `-|>` inheritance (is-a)
  - `..|>` interface implementation (realizes)
  - `-->` association/usage (has-a or uses)
  - `o--` aggregation
- Add `: label` to arrows that cross the control→data boundary (e.g., `: process_audio()`).
- Exclude `[[deprecated]]` type alias stubs (`WavFile`, `MidiFile`, `AudioDevice`, `MidiDevice`) from the diagram — they are compatibility shims, not real components.
- Do **not** invent interfaces that have no corresponding header file in the codebase.

## Step 4 — Summarise changes

After updating the file, provide a short bullet list of what was added, removed, and corrected compared to the previous diagram.
