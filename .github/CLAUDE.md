# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure (generates compile_commands.json)
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build all targets
cmake --build build

# Build specific config (Windows)
cmake --build build --config Debug
cmake --build build --config Release
```

## Test Commands

```bash
# Run all unit tests (Windows)
.\build\tests\unit\Debug\miniaudioengine-unit-tests.exe

# Run all unit tests (Linux)
./build/tests/unit/miniaudioengine-unit-tests

# Run a single test or test suite
.\build\tests\unit\Debug\miniaudioengine-unit-tests.exe --gtest_filter="TrackManager.*"
.\build\tests\unit\Debug\miniaudioengine-unit-tests.exe --gtest_filter="TestClassName.TestMethodName"
```

VS Code tasks in `.vscode/tasks.json` provide "CMake Configure", "CMake Build", and "Run Unit Tests" shortcuts.

## Architecture: Strict Layered System

Each layer is a **static library**. Dependencies flow upward only — lower layers must never depend on upper layers.

```
Layer 4: public / cli / examples  (application interface)
Layer 3: control                  (synchronous, main thread only)
Layer 2: processing               (audio processors; per-processor threading only)
Layer 1: data                     (real-time lock-free callbacks)
Layer 0: framework                (lock-free primitives, logging, utilities)
```

### Layer 3 — Control Plane
Synchronous singletons called from the main thread. `std::mutex` is allowed here.
- `AudioStreamController` (`src/control/audio/`) — audio device management
- `MidiPortController` (`src/control/midi/`) — MIDI port management
- `TrackManager` (`src/public/trackmanager/`) — track lifecycle; owns the `MainTrack` root
- `DeviceManager` (`src/public/io/devicemanager/`) — enumerates audio/MIDI hardware via RtAudio/RtMidi
- `FileManager` (`src/public/io/filemanager/`) — reads/writes WAV and MIDI files via libsndfile

Access all singletons via `::instance()` (e.g., `TrackManager::instance()`).

### Layer 1 — Data Plane (`src/data/`)
Executes inside RtAudio/RtMidi callback threads. Must be **completely lock-free** and finish in **< 1ms**.
- `AudioDataPlane` — per-track audio rendering in RtAudio callback
- `AudioCallbackHandler` — RtAudio callback function + `AudioCallbackContext` struct
- `MidiDataPlane` — per-track MIDI processing in RtMidi callback
- `MidiCallbackHandler` — RtMidi callback function + context struct

### Layer 0 — Framework (`src/framework/include/`)
- `LockfreeRingBuffer<T, Size>` — SPSC lock-free queue; `try_push`/`try_pop`; `memory_order_release`/`acquire`
- `DoubleBuffer<T>` — atomic double-buffer for producer/consumer swap
- `Logger` — thread-safe singleton; use macros `LOG_INFO()`, `LOG_WARNING()`, `LOG_ERROR()`, `LOG_DEBUG()`
- `MessageQueue<T>` — lock-based blocking queue (not used in real-time paths)
- `IController`, `IDataPlane`, `IProcessor`, `IManager` — base interfaces for control/data/processing
- `IInput`, `IDevice`, `IAudioDevice` — shared data models for routing and device metadata
- `RealtimeAssert` — header stub present but not implemented yet

### Track Hierarchy
`MainTrack` is always the root. Regular tracks are direct children of MainTrack (single-level hierarchy). Audio output mixes upward from children to parent.

```
MainTrack (root — owns AudioStreamController, MidiPortController)
├── Track (child — owns AudioDataPlane + MidiDataPlane)
└── Track (child)
```

Each `Track` supports audio/MIDI input from either a hardware device or a file (`std::variant`).

## Namespaces

| Namespace | Layer |
|-----------|-------|
| `miniaudioengine::core` | Framework + Data Plane (Layers 0-1) |
| `miniaudioengine::audio` | Audio control + processing (Layers 2-3) |
| `miniaudioengine::midi` | MIDI control (Layer 3) |
| `miniaudioengine` | Public API managers + CLI (Layer 4) |

## Real-Time Safety Rules (Data Plane)

When writing or modifying any code that executes inside a callback (`AudioDataPlane`, `MidiDataPlane`, callback handlers):

1. **No mutexes** — no `std::mutex`, `std::lock_guard`, or any blocking primitive
2. **No heap allocation** — no `new`, `malloc`, `std::vector::push_back`, or any dynamic allocation
3. **No blocking I/O** — no file reads, no sleeping
4. Use `LockfreeRingBuffer` for inter-thread communication
5. Use `std::atomic` with explicit memory orders for state flags

## Coding Conventions

- Headers under `include/<component>.h`, implementations under `src/<component>.cpp`
- Include guards use `#ifndef __COMPONENT_H__` style
- Test files named `test_<component>_unit.cpp` under `tests/unit/`
- Singleton tests call `TrackManager::instance().clear_tracks()` before each test to reset state
- Platform guards: `#ifdef PLATFORM_WINDOWS` / `#elif defined(PLATFORM_LINUX)`

### Naming

| Element | Convention | Example |
|---|---|---|
| Classes | PascalCase | `AudioDataPlane`, `FileManager` |
| Interfaces/abstract bases | `I` prefix + PascalCase | `IController`, `IDataPlane` |
| Methods | snake_case | `get_output_device()`, `is_running()` |
| Data members | `m_` prefix | `m_running`, `m_stream_state` |
| Pointer members | `p_` prefix | `p_device`, `p_statistics` |
| Enum types | `e` prefix + PascalCase | `eStreamState`, `eMidiMessageType` |
| Enum values | PascalCase | `eStreamState::Playing` |
| `shared_ptr` type aliases | `Ptr` suffix | `IDataPlanePtr`, `AudioDataPlanePtr` |
| Boolean getters | `is_` / `has_` prefix | `is_running()`, `has_audio_input()` |

### Specifiers & Attributes
- `override` on every derived virtual method
- `noexcept` on real-time callbacks and methods that must not throw
- `explicit` on single-parameter constructors
- `const` on all getters and read-only parameters
- `constexpr` / `inline constexpr` for compile-time constants and lookup tables

### Special Member Functions
- Non-copyable types: explicitly `= delete` copy constructor and copy assignment operator
- Base/interface destructors: `virtual ~IFoo() = default`
- Prefer in-class member initializers over constructor body assignment
- `std::make_shared` for all `shared_ptr` construction

### Smart Pointers
- `std::shared_ptr` is the primary ownership type; always alias as `FooPtr`
- `std::weak_ptr` for parent/back references to avoid cycles
- Raw pointers only for non-owning references (e.g., RtAudio callback buffers, `std::ostream*` in Logger)

### Modern C++ Idioms
- `std::optional<T>` for fallible return values instead of out-parameters or sentinel values
- `std::variant` for type-safe discriminated unions (e.g., `SourceVariant`)
- Range-based `for` loops throughout; avoid raw index loops unless index is needed
- `alignas(64)` on `std::atomic` members that occupy hot cache lines
- `thread_local` for per-thread state (e.g., thread name in Logger)

### Comments & Documentation
- All public API declarations use Doxygen: `@brief`, `@param`, `@return`, `@throws`, `@deprecated`
- Inline comments only for non-obvious logic; do not comment self-evident code

## Key Pending Work

- **Processing Plane Orchestration**: `core::IProcessor` uses per-processor threads, but there is no global DSP scheduler yet
- **RealtimeAssert**: Header stubs exist in `framework/include/realtime_assert.h` but not yet implemented
- **CLI application**: Basic implementation exists but is not yet a finished application

## Dependencies (vcpkg)

`rtaudio`, `rtmidi`, `libsndfile`, `gtest` — managed via `vcpkg.json`. Windows uses vcpkg `find_package`; Linux falls back to `pkg-config`.
