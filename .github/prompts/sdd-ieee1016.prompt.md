---
description: "Generate a Software Design Description (SDD) document for the miniaudioengine project following the IEEE 1016-2009 standard. Reads source headers, architecture review, and project docs to produce a complete, structured markdown SDD."
name: "IEEE 1016 Software Design Description"
argument-hint: "[optional: focus component or viewpoint, e.g. 'AudioDataPlane' or 'interface viewpoint']"
agent: "agent"
tools: [search/codebase, read, execute, edit/createFile]
---

Produce a **Software Design Description (SDD)** for the miniaudioengine project that conforms to **IEEE Std 1016-2009**.

Reference files to read first:
- [copilot-instructions.md](../copilot-instructions.md)
- Root [CMakeLists.txt](../../CMakeLists.txt) and [vcpkg.json](../../vcpkg.json) for dependency and build context.

Then explore the source tree:
- `src/framework/` — shared framework layer (accessible by all layers): interfaces, primitives, Device, File
- `src/data/` — Layer 1 real-time data plane
- `src/processing/` — Layer 2 audio processors
- `src/control/` — Layer 3 controllers
- `src/public/` — Layer 4 public API

---

## Output Format

Produce a single Markdown document with the sections below. Use level-2 headings for each IEEE 1016 section. Preserve numbering from the standard. Only emit a section if there is actual design content to describe; omit empty stubs.

If `$ARGS` targets a specific component or viewpoint, narrow the design views to that scope and note the scope at the top.

---

## Section 1 — Introduction

### 1.1 Purpose
Describe the purpose of this SDD: to define the design of the miniaudioengine real-time audio/MIDI engine in sufficient detail for implementation and verification.

### 1.2 Scope
Name the system, summarise what it does (real-time audio capture/playback, MIDI I/O, file-based audio, layered C++20 architecture), and state what is out of scope.

### 1.3 Context
Summarise the operational environment: desktop/embedded C++20, Windows & Linux targets, RtAudio + RtMidi + libsndfile as hardware abstraction, vcpkg dependency management.

### 1.4 Design Document Overview
List which IEEE 1016 viewpoints are addressed and in which section they appear.

### 1.5 Glossary / Definitions
Define domain-specific terms (real-time callback, data plane, control plane, ring buffer, data plane, MIDI message, track hierarchy, etc.).

---

## Section 2 — Design Stakeholders and Their Concerns

Enumerate the stakeholders and map each to a design concern and the viewpoint(s) that address it.

| Stakeholder | Role | Key Concern | Addressed By |
|-------------|------|-------------|--------------|
| Application developer | Integrator | Public API usability | Interface Viewpoint |
| Real-time audio subsystem | Runtime | RT-safe callback execution | Composition, Interaction Viewpoints |
| ... | ... | ... | ... |

Fill the table by reading the public API (`src/public/`) and the real-time rules in the architecture guide.

---

## Section 3 — Design Views

Produce one sub-section per viewpoint below. For each viewpoint:
1. State the **design concern** it addresses.
2. List the **design elements** (classes, interfaces, modules) covered.
3. Provide a concise **design description** with a PlantUML or Mermaid diagram where structural or behavioural relationships are non-trivial.

### 3.1 Context Viewpoint
Show the system boundary: miniaudioengine as a black box interacting with audio hardware (RtAudio), MIDI hardware (RtMidi), the file system (libsndfile), and the host application.

### 3.2 Composition Viewpoint (Structural)
Describe how the system decomposes into the 5 layers. Show the directory-to-layer mapping, namespaces, and which layer each major class belongs to. Include the full component hierarchy diagram from `ARCHITECTURE_REVIEW.md`.

### 3.3 Logical Viewpoint (Class Design)
For each layer describe its key abstractions:
- **Framework (shared)**: `IController`, `IDataPlane`, `IManager`, `IDevice`, `LockfreeRingBuffer<T,N>`, `DoubleBuffer<T>`, `Logger`, `Device`, `File`
- **Layer 1**: `AudioDataPlane`, `MidiDataPlane`, callback handlers
- **Layer 2**: `IAudioProcessor`, `Sample`, `SamplePlayer`
- **Layer 3**: `AudioStreamController`, `MidiPortController`
- **Layer 4**: `TrackManager`, `MainTrack`, `Track`, `DeviceManager`, `FileManager`, `WavFile`, `MidiFile`

For each class, read the corresponding header and document: responsibilities, key methods (signatures), and important member fields. Highlight ownership and smart-pointer types ( `shared_ptr` aliases ending in `Ptr`).

### 3.4 Dependency Viewpoint
Reproduce the layer dependency matrix. Document allowed versus forbidden cross-layer `#include` relationships. Enumerate the 10 violations found in `ARCHITECTURE_REVIEW.md` and state their planned remediation.

### 3.5 Information Viewpoint (Data Design)
Describe the major data structures and how data flows between layers:
- Audio sample buffer lifecycle (RtAudio callback → `AudioDataPlane` → `IAudioProcessor`)
- MIDI message lifecycle (RtMidi callback → `MidiDataPlane` → `LockfreeRingBuffer` → `MidiPortController`)
- File-backed audio (`WavFile` preload → span/buffer → `AudioDataPlane`)
- Track state (`eStreamState` transitions)

### 3.6 Interface Viewpoint
For each public API class in `src/public/` and each interface in `src/framework/`, document:
- Interface name and namespace
- Method signatures (read from headers)
- Preconditions, postconditions, thread-safety guarantees
- Deprecation notes (e.g., `IAudioController` → `AudioStreamController`)

### 3.7 Pattern Use Viewpoint
Document recurring design patterns:
- **Singleton** (`TrackManager::instance()`, `DeviceManager::instance()`, etc.)
- **Observer / Callback** (RtAudio/RtMidi raw callbacks delegating to data planes)
- **Strategy** (`IAudioProcessor` polymorphism)
- **Double Buffer / Ring Buffer** (lock-free RT communication)
- **Variant** (`SourceVariant` in `Track` for device vs. file sources)

### 3.8 Interaction Viewpoint (Behavioural)
Describe the key runtime scenarios as sequence descriptions or Mermaid sequence diagrams:

1. **Start audio playback** — host calls `TrackManager` → `AudioStreamController::start()` → RtAudio opens stream → `AudioDataPlane` callback loop begins.
2. **MIDI input event** — RtMidi fires callback → `MidiCallbackHandler` constructs `MidiMessage` → pushed to `LockfreeRingBuffer` → `MidiPortController` polls and dispatches.
3. **File-backed playback** — `FileManager` loads `WavFile` → decoded PCM preloaded into `AudioDataPlane` buffer → playback mixed into output.

### 3.9 State Dynamics Viewpoint
Document the `eStreamState` state machine for `AudioStreamController`:
- States: `Idle`, `Playing`, `Paused`, `Stopped`, `Error`
- Allowed transitions and triggering operations
- Represent as a Mermaid stateDiagram-v2

### 3.10 Resource Viewpoint
Describe real-time resource constraints:
- Callback budget: ≤ 1 ms total per callback
- Memory: no heap allocation in RT path; pre-allocated ring buffers and double buffers
- Thread model: RT audio thread (RtAudio callback), RT MIDI thread (RtMidi callback), main control thread
- Lock-free synchronisation: `LockfreeRingBuffer`, `std::atomic`, `alignas(64)` to avoid false sharing

---

## Section 4 — Design Rationale

For each significant design decision, state the decision, the alternatives considered, and the reasons for the choice:

| Decision | Alternatives Considered | Rationale |
|----------|------------------------|-----------|
| `LockfreeRingBuffer` for RT↔control messaging | `std::queue` + mutex; `boost::lockfree` | Avoids mutex in RT; stays header-only; no external dependency |
| `std::shared_ptr` as primary ownership type | raw pointers; `unique_ptr` | Simplifies shared ownership across manager/controller/plane boundaries |
| Layered architecture with strict upward-only deps | Flat module graph | Enforces RT-safety by keeping RT callbacks free of control/public-layer code |
| vcpkg for dependency management | Conan; submodules | Familiar to Windows devs; integrates cleanly with CMake toolchain |

Augment this table with additional decisions discovered when reading the codebase.

---

## Section 5 — Design Overlays

Document cross-cutting concerns that apply across multiple viewpoints:

### 5.1 Real-Time Safety Overlay
Rules enforced in `src/data/` callbacks: no mutexes, no heap allocation, no blocking I/O, max 1 ms callback work. Identify which specific code paths are covered and how violations in `ARCHITECTURE_REVIEW.md` affect RT safety.

### 5.2 Platform Portability Overlay
`PLATFORM_WINDOWS` / `PLATFORM_LINUX` preprocessor guards. List files that contain conditional compilation and the specific divergences.

### 5.3 Error Handling Overlay
Policy: validate at system boundaries (user input, file I/O, device open); never in RT callbacks. Document how errors surface (return codes, `std::optional`, logging).

### 5.4 Testability Overlay
Mock strategy: `tests/mocks/include/` mirrors interface names with `Mock` prefix under namespace `miniaudioengine::test`. Singletons reset via `::clear_tracks()` / equivalent. WHOLEARCHIVE linking for `gtest`.

---

## Section 6 — Design Languages

State the languages and notations used in this SDD:
- **Markdown** — prose structure
- **C++20** — implementation language
- **PlantUML / Mermaid** — structural and behavioural diagrams
- **CMake** — build and configuration language
- **IEEE 1016-2009** — SDD standard governing this document

---

*End of SDD template. Populate each section from the actual codebase. Do not invent details not present in the source files or architecture review.*
