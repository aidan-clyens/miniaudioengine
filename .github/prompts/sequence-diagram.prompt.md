---
description: "Trace a specific operation (e.g. play_track, open_file, stop_track, MIDI input) through the miniaudioengine layers and render it as a Mermaid sequence diagram, covering both the control plane and the real-time data plane."
agent: "agent"
argument-hint: "<operation> — e.g. play_track, stop_track, open_file, monitor_input, midi_input"
---

Trace the **`${argument}`** operation through the miniaudioengine layer stack and produce a Mermaid `sequenceDiagram` that covers both the **control plane** and, where applicable, the **real-time data plane**.

## Step 1 — Identify the operation

Map `${argument}` to its entry point and affected layers:

| Operation | Entry point | Layers touched |
|-----------|-------------|----------------|
| `play_track` | `Track::play()` in services | Services → Engine (`AudioController`) → Adapter (`AudioAdapter` / RtAudio) |
| `stop_track` | `Track::stop()` in services | Services → Engine → Adapter |
| `open_file` | `FileService::open_file()` | Services → Adapter (`FileAdapter` / libsndfile) |
| `monitor_input` | `Track::play()` with device source | Services → Engine → Adapter (RtAudio input stream) |
| `midi_input` | `Track::set_midi_handler()` + `MainTrack::play()` | Services → Engine (`MidiController`) → Adapter (`MidiAdapter` / RtMidi) |
| `audio_callback` | RtAudio callback → `MainTrack::audio_callback()` | Engine data plane → `AudioDataPlane` → processing chain |
| `midi_callback` | RtMidi callback → MIDI message dispatch | Engine data plane → `MidiDataPlane` → handler |

If `${argument}` is not in the table above, infer the entry point from the closest matching public API method and the layer reference below.

## Step 2 — Read the relevant source files

Locate the implementation for the operation:

- **Public API / services**: `src/services/`, `src/cli/`
- **Engine control plane**: `src/engine/include/`, `src/engine/src/`
- **Engine data plane / real-time callbacks**: `src/engine/` — look for `audio_callback`, `midi_callback`, `process_audio`
- **Adapters**: `src/adapters/include/`, `src/adapters/src/`
- **Core interfaces**: `src/framework/include/`
- **Design reference**: `docs/DESIGN_DOC.md` — section 3.7 Interaction View contains existing sequence diagrams

For each call or method invocation along the path, record:
- **Caller** and **callee** (class names, not file names)
- **Method signature** (abbreviated: `method(key_params)`)
- Whether the call is **synchronous**, **asynchronous** (callback), or crosses a **thread boundary**
- Any **conditional branches** (e.g. file source vs. device source, preloaded vs. live)
- Any **lock-free ring buffer** enqueue/dequeue operations that cross threads

## Step 3 — Produce the Mermaid diagram(s)

### Control plane diagram

Render a `sequenceDiagram` for the **control-plane path** (main thread, service layer → engine → adapter):

```
sequenceDiagram
    participant App
    participant <ServiceClass>
    participant <EngineClass>
    participant <AdapterClass>
    participant <ExternalLib>
```

Conventions:
- Use `->>`  for synchronous calls, `-->>`  for return values
- Use `-->>` (dashed) for callbacks and async responses
- Wrap conditional branches in `opt` or `alt … else` blocks with a descriptive label (e.g. `alt File source`, `opt Preloaded`)
- Mark real-time constraint boundaries with a `Note over` annotation: `Note over X,Y: Real-time callback thread`
- Do **not** show internal lock/unlock details; they belong in comments, not sequence steps

### Data plane diagram (if applicable)

If the operation involves a real-time callback (audio or MIDI), produce a **second** `sequenceDiagram` for the **data plane**:

- Start from the external library callback (RtAudio, RtMidi)
- Show the `MainTrack` dispatch loop: `loop For each registered Track`
- Show buffer operations: copy, mix, read position advance
- Show the processing chain: `loop For each non-bypassed processor`
- End with the return value back to the external library

### Diagram labels
- Title each diagram with a Markdown heading: `#### Control Plane` and `#### Data Plane`
- Keep participant names short and matching class names in the codebase

## Step 4 — Annotate thread boundaries

After the diagram(s), add a brief Markdown table listing every **thread boundary** crossed:

| Transition | From thread | To thread | Mechanism |
|-----------|-------------|-----------|-----------|
| e.g. `AudioController → RtAudio callback` | Main / control thread | RtAudio callback thread | RtAudio `openStream` callback |
| e.g. ring buffer enqueue | Control thread | Callback thread | `LockfreeRingBuffer<T, Size>` |

## Layer reference

| Layer | Path | Namespace | Real-time? |
|-------|------|-----------|------------|
| 4 Services | `src/services/` | `miniaudioengine` | No |
| 3 Engine | `src/engine/` | `miniaudioengine::audio` / `::midi` | **Callbacks yes** |
| 2 Processing | `src/processing/` | `miniaudioengine::audio` | No |
| 1 Adapters | `src/adapters/` | `miniaudioengine` | No |
| 0 Core/framework | `src/framework/` | `miniaudioengine::framework` | No |

**Real-time safety reminder**: any code path that touches `src/engine/` callbacks must obey the four rules — no mutexes, no heap allocation, no blocking I/O, and use `LockfreeRingBuffer` for cross-thread messaging.
