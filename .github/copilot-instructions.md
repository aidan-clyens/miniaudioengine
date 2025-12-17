# Minimal Audio Engine - AI Coding Agent Instructions

## Project Overview
Cross-platform C++20 audio processing engine designed for embedded and desktop environments. Emphasizes thread-safe programming, concurrency patterns, and modern C++ best practices.

## Architecture

### Component Hierarchy
```
framework (base layer)
├── coreengine (orchestrator)
│   ├── audioengine (RtAudio wrapper)
│   │   ├── trackmanager (audio/MIDI track management)
│   │   └── devicemanager (hardware abstraction)
│   ├── midiengine (RtMidi wrapper)
│   └── filemanager (libsndfile wrapper)
└── cli (user interface - CLI11 + replxx)
```

**Key insight**: Each component is a static library. Dependencies flow upward - lower layers never depend on upper layers. `coreengine` coordinates all subsystems.

### Threading & Concurrency Pattern
All engines inherit from `IEngine<T>` template (see [framework/include/engine.h](../src/framework/include/engine.h)):
- Each engine runs in its own `std::jthread` 
- Communication via thread-safe `MessageQueue<T>` (lock-free push/blocking pop)
- Engines implement `run()` (main loop) and `handle_messages()` (message processing)
- Thread lifecycle: `start_thread()` waits for thread readiness with timeout, `stop_thread()` signals shutdown

**Example**: [coreengine.h](../src/coreengine/include/coreengine.h) defines `CoreEngineMessage` enum for inter-thread communication.

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
- **Library targets**: All src subdirectories build static libraries with `FILE_SET HEADERS`

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

# Run application
./build/src/EmbeddedAudioEngine

# Run tests
./build/tests/unit/EmbeddedAudioEngineUnitTests
```

**VS Code tasks**: Use tasks.json for "CMake Configure", "CMake Build", "Run", and "Run Units Test".

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
- GTest framework in `tests/unit/`
- Tests use singletons: `TrackManager::instance()`, `DeviceManager::instance()`
- Clear state with `.clear_tracks()` before tests
- Test files named `test_<component>_unit.cpp`

## Common Tasks

### Adding a New Engine Component
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
- See [test_track_unit.cpp](../tests/unit/test_track_unit.cpp) for usage patterns

## Docker Development
- Dockerfile provides reproducible Linux build environment
- Scripts in `docker/` for multi-arch builds (x86_64, ARM64)
- CI/CD uses GitHub Actions for automated builds

## Critical Files to Understand
- [framework/include/engine.h](../src/framework/include/engine.h) - Base threading model
- [framework/include/messagequeue.h](../src/framework/include/messagequeue.h) - Inter-thread communication
- [coreengine/include/coreengine.h](../src/coreengine/include/coreengine.h) - System orchestrator
- [src/CMakeLists.txt](../src/CMakeLists.txt) - Component dependency graph
