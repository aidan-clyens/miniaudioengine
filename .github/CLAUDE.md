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

## Architecture: Strict 3-Plane Layered System

Each layer is a **static library**. Dependencies flow upward only — lower layers must never depend on upper layers.

```
Layer 4: cli / examples           (application interface)
Layer 3: control                  (synchronous, main thread only)
Layer 2: processing               (NOT YET IMPLEMENTED — future background DSP workers)
Layer 1: data                     (real-time lock-free callbacks)
Layer 0: framework                (lock-free primitives, logging, utilities)
```

### Layer 3 — Control Plane (`src/control/`)
Synchronous singletons called from the main thread. `std::mutex` is allowed here.
- `AudioStreamController` — audio device management (`start_stream`, `stop_stream`, `set_output_device`)
- `MidiPortController` — MIDI port management (`open_input_port`, `close_input_port`)
- `TrackManager` — track lifecycle; owns the `MainTrack` root
- `DeviceManager` — enumerates audio/MIDI hardware via RtAudio/RtMidi
- `FileManager` — reads/writes WAV and MIDI files via libsndfile

Access all singletons via `::instance()` (e.g., `TrackManager::instance()`).

### Layer 1 — Data Plane (`src/data/`)
Executes inside RtAudio/RtMidi callback threads. Must be **completely lock-free** and finish in **< 1ms**.
- `TrackAudioDataPlane` — per-track audio rendering in RtAudio callback
- `AudioCallbackHandler` — pure RtAudio callback function + `AudioCallbackContext` struct
- `TrackMidiDataPlane` — per-track MIDI processing in RtMidi callback
- `MidiCallbackHandler` — pure RtMidi callback function + context struct

### Layer 0 — Framework (`src/framework/include/`)
- `LockfreeRingBuffer<T, Size>` — SPSC lock-free queue; `try_push`/`try_pop`; `memory_order_release`/`acquire`
- `DoubleBuffer<T>` — atomic double-buffer for producer/consumer swap
- `Logger` — thread-safe singleton; use macros `LOG_INFO()`, `LOG_WARNING()`, `LOG_ERROR()`, `LOG_DEBUG()`
- `MessageQueue<T>` — lock-based blocking queue (legacy pattern, being phased out)
- `IEngine<T>` — legacy threading base class using `std::jthread` (new components do NOT use this)
- `Subject<T>` / `Observer<T>` — observer pattern with `std::weak_ptr` to avoid cycles

### Track Hierarchy
`MainTrack` is always the root. Regular tracks are direct children of MainTrack (single-level hierarchy). Audio output mixes upward from children to parent.

```
MainTrack (root — owns AudioStreamController, MidiPortController)
├── Track (child — owns TrackAudioDataPlane + TrackMidiDataPlane)
└── Track (child)
```

Each `Track` supports audio/MIDI input from either a hardware device or a file (`std::variant`).

## Namespaces

| Namespace | Layer |
|-----------|-------|
| `miniaudioengine::core` | Framework (Layer 0) |
| `miniaudioengine::data` | Data Plane (Layer 1) |
| `miniaudioengine::control` | Control Plane (Layer 3) |
| `miniaudioengine::processing` | Processing Plane (Layer 2) |
| `miniaudioengine::file` | File I/O |

## Real-Time Safety Rules (Data Plane)

When writing or modifying any code that executes inside a callback (`TrackAudioDataPlane`, `TrackMidiDataPlane`, callback handlers):

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

## Key Pending Work

- **Processing Plane (Layer 2)**: Not yet implemented — planned for background DSP worker threads
- **RealtimeAssert**: Header stubs exist in `framework/include/realtime_assert.h` but not yet implemented
- **CLI application**: Basic implementation exists but is not yet a finished application

## Dependencies (vcpkg)

`rtaudio`, `rtmidi`, `libsndfile`, `gtest` — managed via `vcpkg.json`. Windows uses vcpkg `find_package`; Linux falls back to `pkg-config`.
