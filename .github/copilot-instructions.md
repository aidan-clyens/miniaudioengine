# Minimal Audio Engine - AI Coding Agent Instructions

## Project Overview
Cross-platform C++20 audio processing engine designed for embedded and desktop environments. Emphasizes thread-safe programming, concurrency patterns, real-time audio safety, and modern C++ best practices.

## Architecture

### 3-Plane Architecture (Partial Implementation)
The project is transitioning to a strict 3-plane architecture with clear separation:

```
Layer 4: cli (application interface - basic CLI implementation)
         ↓
Layer 3: controlplane (synchronous control operations)
         ├── audio (AudioStreamController - device management)
         ├── trackmanager (Track lifecycle and routing)
         ├── devicemanager (hardware enumeration)
         └── filemanager (disk I/O - libsndfile wrapper)
         ↓
Layer 2: processingplane (NOT YET IMPLEMENTED - planned for background worker threads)
         ↓
Layer 1: dataplane (lock-free real-time components)
         ├── audio (TrackAudioDataPlane, AudioCallbackHandler)
         └── midi (MidiEngine - legacy threading, miditypes, midicontroller)
         ↓
Layer 0: framework (lock-free primitives, logging, utilities)
```

**Key principles**:
- **Control Plane**: Synchronous operations from main thread, locks allowed, infrequent calls
- **Data Plane**: Real-time callbacks (RtAudio/RtMidi), lock-free only, < 1ms execution
- **Processing Plane**: Background worker threads for CPU-intensive DSP (NOT YET IMPLEMENTED)
- Each layer is a static library; dependencies flow upward only
- Lower layers NEVER depend on upper layers

### Threading & Concurrency Pattern

#### Control Plane (Synchronous)
Control plane components like `AudioStreamController` and `TrackManager` are **synchronous singletons** called from the main thread:
- No dedicated threads (unlike the old `IEngine` pattern)
- Operations block until complete (e.g., `start_stream()`, `stop_stream()`)
- Can use `std::mutex` for thread safety where needed
- Example: [audiostreamcontroller.h](../src/controlplane/audio/include/audiostreamcontroller.h)

#### Data Plane (Real-Time Callbacks)
Data plane components execute in real-time audio/MIDI callback threads:
- **Lock-free only** - no mutexes, no allocations, no blocking
- Use lock-free ring buffers for inter-thread communication
- `TrackAudioDataPlane` renders audio in RtAudio callback thread
- `MidiEngine` processes MIDI (still uses legacy threading pattern - to be refactored)
- **Critical**: Must complete in < 1ms to avoid dropouts

#### Framework Lock-Free Primitives
[lockfree_ringbuffer.h](../src/framework/include/lockfree_ringbuffer.h) provides SPSC queue:
- Single-producer single-consumer lock-free ring buffer
- Used for MIDI event passing, audio buffer passing
- Template: `LockfreeRingBuffer<T, Size>`
- Memory ordering: `memory_order_release` on write, `memory_order_acquire` on read

#### Legacy Threading Pattern (Being Phased Out)
Old components inherited from `IEngine<T>` template ([engine.h](../src/framework/include/engine.h)):
- Each engine runs in dedicated `std::jthread`
- Communication via `MessageQueue<T>` (lock-free push/blocking pop)
- Thread lifecycle: `start_thread()` waits for readiness, `stop_thread()` signals shutdown
- **Note**: New control plane components do NOT use this pattern

### Observer Pattern
Framework provides generic Observer/Subject implementation:
- `Subject<T>` manages `std::weak_ptr<Observer<T>>` to avoid circular references
- Thread-safe with `std::mutex` guards on attach/detach/notify
- Used for decoupled event propagation between components

## Build System

### CMake Structure
- **C++20 required**: `CMAKE_CXX_STANDARD 20`
- **Platform detection**: `PLATFORM_WINDOWS` or `PLATFORM_LINUX` defines set automatically
- **Dependency management**:
  - Windows: vcpkg (`find_package(RtAudio CONFIG REQUIRED)`)
  - Linux: pkg-config fallback for system libraries
- **Library targets**: All src subdirectories build static libraries
- **Current structure**:
  ```
  src/
  ├── framework/           (Layer 0 - base primitives)
  │   └── include/
  │       ├── engine.h               (Legacy threading pattern - IEngine<T>)
  │       ├── messagequeue.h         (Thread-safe message queue)
  │       ├── lockfree_ringbuffer.h  (Lock-free SPSC queue)
  │       ├── doublebuffer.h         (Atomic double-buffer)
  │       ├── realtime_assert.h      (RT safety stubs - not yet implemented)
  │       ├── logger.h               (Thread-safe logging)
  │       ├── observer.h/subject.h   (Observer pattern)
  │       └── input.h                (Input handling utilities)
  ├── dataplane/          (Layer 1 - real-time components)
  │   ├── audio/
  │   │   ├── audiodataplane.h  (Per-track audio rendering)
  │   │   └── audiocallbackhandler.h (RtAudio callback wrapper)
  │   └── midi/
  │       ├── midiengine.h           (Legacy IEngine pattern - needs refactor)
  │       ├── miditypes.h            (MIDI message types)
  │       └── midicontroltypes.h       (MIDI CC definitions)
  ├── controlplane/       (Layer 3 - synchronous control)
  │   ├── audio/          (AudioStreamController - refactored)
  │   ├── trackmanager/   (Track, TrackManager)
  │   ├── devicemanager/  (Device enumeration)
  │   └── filemanager/    (WAV/MIDI file I/O)
  └── cli/                (Layer 4 - basic CLI implementation)
      ├── include/cli.h
      └── src/cli.cpp
  ```

### Key Dependencies (vcpkg.json)
- `rtaudio`: Cross-platform audio I/O
- `rtmidi`: Cross-platform MIDI I/O  
- `libsndfile`: Audio file read/write (WAV, FLAC, etc.)
- `gtest`: Unit testing
- `cli11`: Command-line parsing
- `replxx`: Interactive REPL with history

### Build Workflow
```bash
# Configure (creates compile_commands.json)
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
cmake --build build

# Run tests
./build/tests/unit/EmbeddedAudioEngineUnitTests
```

**VS Code tasks**: Use tasks.json for "CMake Configure", "CMake Build", "Run", and "Run Units Test".

**Note**: The CLI application is currently under development - the main executable target is not yet finalized.

## Coding Conventions

### File Organization
- **Headers**: `include/<component>.h` with public API
- **Implementation**: `src/<component>.cpp` for private logic
- **Namespace**: Everything in `MinimalAudioEngine`
- **Include guards**: `#ifndef __COMPONENT_H__` style

### Thread Safety Guidelines
1. Use `std::atomic<bool>` with memory orders (`std::memory_order_acquire`/`release`) for flags
2. `std::mutex` + `std::lock_guard` for critical sections
3. Avoid shared state - prefer message passing via `MessageQueue`
4. Set thread names via `set_thread_name()` for debugging

### Logging
Use `LOG_INFO()`, `LOG_ERROR()`, `LOG_WARNING()` macros from [framework/include/logger.h](../src/framework/include/logger.h). Thread-safe by design.

### Testing
- **Unit Tests** (`tests/unit/`):
  - GTest framework
  - Tests use singletons: `TrackManager::instance()`, `DeviceManager::instance()`
  - Clear state with `.clear_tracks()` before tests
  - Test files named `test_<component>_unit.cpp`
  - Components tested: AudioStreamController, DeviceManager, FileManager, TrackManager, Track, TrackAudioDataPlane, LockfreeRingBuffer, DoubleBuffer, MessageQueue, Observer/Subject
- **Profiling Tests** (`tests/profiling/`):
  - `test_lockfree_ringbuffer_profiling.cpp`: Single producer/consumer throughput at various sample rates
  - `test_track_wav_file_profiling.cpp`: WAV file playback performance
  - Python analysis scripts for results visualization
- **Integration Tests** (`tests/integration/`): Directory exists but not yet populated

## Common Tasks

### Working with the CLI
The CLI component provides basic command-line argument parsing:
- **Files:** `cli/include/cli.h`, `cli/src/cli.cpp`
- **Usage:** Define `Command` structs with argument names, short names, descriptions, and action callbacks
- **Features:** Help text generation, version display
- **Examples:** See `examples/wav-audio-player/src/main.cpp` and `examples/midi-device-input/src/main.cpp`
- **Note:** Basic implementation - not a full interactive shell (despite vcpkg dependency on replxx)

### Working with Control Plane Components
Control plane components (AudioStreamController, TrackManager, DeviceManager, FileManager) are synchronous singletons:
- Access via `::instance()` method (e.g., `AudioStreamController::instance()`)
- Methods are synchronous and block until complete
- Safe to call from main thread only
- Use `std::mutex` internally where needed for thread safety

### Working with Data Plane Components
Data plane components execute in real-time callbacks:
- `TrackAudioDataPlane` renders audio in RtAudio callback
- Must be lock-free - no mutexes, no allocations
- Use `LockfreeRingBuffer` for inter-thread communication
- Keep execution time < 1ms to avoid audio dropouts

### Adding a New Engine Component (Legacy Pattern)
**Note**: This pattern is being phased out for control plane. Use only for future processing plane workers.
1. Create `src/<name>/CMakeLists.txt` with static library target
2. Define `<Name>Message` struct with enum for message types
3. Inherit from `IEngine<<Name>Message>`
4. Implement `run()` loop and `handle_messages()` switch
5. Link dependent libraries in CMakeLists.txt
6. Add to `src/CMakeLists.txt` with `add_subdirectory(<name>)`

### Platform-Specific Code
Use preprocessor checks:
```cpp
#ifdef PLATFORM_WINDOWS
  // Windows-specific
#elif defined(PLATFORM_LINUX)
  // Linux-specific
#endif
```

### Working with Tracks
- `TrackManager::instance()` is singleton for all track operations
- Tracks support audio/MIDI input/output via device assignment
- Each `Track` owns a `TrackAudioDataPlane` for real-time audio rendering
- Audio input can be from `AudioDevice` or `WavFile`
- MIDI input can be from `MidiDevice` or `MidiFile`
- See [test_track_unit.cpp](../tests/unit/test_track_unit.cpp) for usage patterns

### Real-Time Safety Guidelines
When working in the data plane:
1. **Never** use `std::mutex` or any blocking synchronization
2. **Never** allocate memory (`new`, `malloc`, `std::vector::push_back`)
3. **Never** call blocking I/O operations
4. Use `LockfreeRingBuffer` for inter-thread communication
5. Use `std::atomic` with appropriate memory orders for state flags
6. Keep execution time < 1ms

## Docker Development
- Dockerfile provides reproducible Linux build environment
- Scripts in `docker/` for multi-arch builds (x86_64, ARM64)
- CI/CD uses GitHub Actions for automated builds

## Refactor Status

### Completed Components
- ✅ **Framework**: 
  - Lock-free primitives: `LockfreeRingBuffer<T, Size>` (SPSC queue), `DoubleBuffer<T>` (atomic swap)
  - Logging: `Logger` singleton with thread-safe file/console output
  - Threading: `IEngine<T>` template (legacy pattern for workers)
  - Observer pattern: `Subject<T>` / `Observer<T>` (non-real-time use)
  - Message passing: `MessageQueue<T>` (lock-based, blocking pop)
- ✅ **Data Plane Audio**: 
  - `TrackAudioDataPlane`: Per-track audio rendering, WAV file playback
  - `AudioCallbackHandler`: Pure RtAudio callback function with context struct
- ✅ **Data Plane MIDI**: 
  - `MidiEngine`: MIDI port management (legacy `IEngine<T>` pattern)
  - MIDI types: `MidiMessage`, `MidiNoteMessage`, `MidiControlMessage`
  - `midicontroltypes.h`: Enum definitions for MIDI CC numbers (Novation Launchkey)
- ✅ **Control Plane Audio**: 
  - `AudioStreamController`: Synchronous audio device management (refactored from AudioEngine)
- ✅ **Control Plane Managers**: 
  - `TrackManager`: Track lifecycle management
  - `DeviceManager`: Audio/MIDI device enumeration
  - `FileManager`: WAV/MIDI file I/O (libsndfile wrapper)
- ✅ **Track**: 
  - Supports both device and file input (audio and MIDI)
  - Owns `TrackAudioDataPlane` instance
  - Implements Observer pattern (legacy - to be removed)
  - Play/stop control, event callbacks
- ✅ **CLI**: Basic command-line interface implementation
- ✅ **Examples**: 
  - `wav-audio-player`: WAV file playback example
  - `midi-device-input`: MIDI device input example
- ✅ **Tests**: 
  - Unit tests: All major components
  - Profiling tests: `LockfreeRingBuffer`, Track WAV playback

### In Progress / TODO
- ⚠️ **Processing Plane**: NOT IMPLEMENTED - planned for background DSP workers
- ⚠️ **RealtimeAssert**: Header exists with stub macros - implementation pending
- ⚠️ **MidiEngine Refactor**: Currently in dataplane but still uses legacy `IEngine<T>` pattern with dedicated thread and `MessageQueue`. Needs refactoring to either:
  - Control plane: `MidiPortController` (synchronous, like AudioStreamController)
  - Data plane: Lock-free callback handler (like AudioCallbackHandler)
- ⚠️ **Track Refactor**: Still uses `Observer` pattern and mutex - should be control-plane only
- ⚠️ **CLI Application**: Basic implementation exists but not yet a full application

See [ARCHITECTURE_REFACTOR_GUIDE.md](../ARCHITECTURE_REFACTOR_GUIDE.md) for detailed migration plan.

## Critical Files to Understand

### Framework (Layer 0)
- [framework/include/engine.h](../src/framework/include/engine.h) - Legacy threading model (`IEngine<T>` template)
- [framework/include/messagequeue.h](../src/framework/include/messagequeue.h) - Thread-safe message queue
- [framework/include/lockfree_ringbuffer.h](../src/framework/include/lockfree_ringbuffer.h) - Lock-free SPSC queue
- [framework/include/doublebuffer.h](../src/framework/include/doublebuffer.h) - Atomic double-buffer for producer/consumer
- [framework/include/realtime_assert.h](../src/framework/include/realtime_assert.h) - RT safety macros (stubs)
- [framework/include/logger.h](../src/framework/include/logger.h) - Thread-safe logging

### Data Plane (Layer 1)
- [dataplane/audio/include/audiodataplane.h](../src/dataplane/audio/include/audiodataplane.h) - Per-track audio rendering
- [dataplane/audio/include/audiocallbackhandler.h](../src/dataplane/audio/include/audiocallbackhandler.h) - RtAudio callback wrapper
- [dataplane/midi/include/midiengine.h](../src/dataplane/midi/include/midiengine.h) - MIDI engine (legacy pattern, needs refactor)
- [dataplane/midi/include/miditypes.h](../src/dataplane/midi/include/miditypes.h) - MIDI message types
- [dataplane/midi/include/midicontroltypes.h](../src/dataplane/midi/include/midicontroltypes.h) - MIDI CC definitions

### Control Plane (Layer 3)
- [controlplane/audio/include/audiostreamcontroller.h](../src/controlplane/audio/include/audiostreamcontroller.h) - Audio device control (refactored)
- [controlplane/trackmanager/include/track.h](../src/controlplane/trackmanager/include/track.h) - Track management
- [controlplane/trackmanager/include/trackmanager.h](../src/controlplane/trackmanager/include/trackmanager.h) - Track collection manager
- [controlplane/devicemanager/include/devicemanager.h](../src/controlplane/devicemanager/include/devicemanager.h) - Device enumeration
- [controlplane/filemanager/include/filemanager.h](../src/controlplane/filemanager/include/filemanager.h) - File I/O

### Build System
- [src/CMakeLists.txt](../src/CMakeLists.txt) - Top-level component dependency graph
- [vcpkg.json](../vcpkg.json) - Dependency manifest
