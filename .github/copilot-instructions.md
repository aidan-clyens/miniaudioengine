# Project Guidelines

## Code Style
- C++20 required; keep layer-specific namespaces aligned with the current codebase:
	- `miniaudioengine::core` for framework primitives and data planes
	- `miniaudioengine::audio` for audio control/processing types
	- `miniaudioengine::midi` for MIDI control types
	- `miniaudioengine` (root) for public API managers and CLI
- Public headers in `include/<component>.h`, implementations in `src/<component>.cpp`, include guards use `#ifndef __COMPONENT_H__` style.
- Logging via `LOG_INFO()`, `LOG_WARNING()`, `LOG_ERROR()`, `LOG_DEBUG()` in `src/framework/include/logger.h`.

### Naming Conventions
- Classes: PascalCase — `AudioDataPlane`, `FileManager`
- Interfaces/abstract bases: `I` prefix + PascalCase — `IController`, `IDataPlane`
- Methods: snake_case — `get_output_device()`, `is_running()`
- Data members: `m_` prefix — `m_running`, `m_stream_state`
- Pointer members: `p_` prefix — `p_device`, `p_statistics`
- Enum types: `e` prefix + PascalCase — `eStreamState`, `eMidiMessageType`
- Enum values: PascalCase — `eStreamState::Playing`
- `shared_ptr` type aliases: `Ptr` suffix — `IDataPlanePtr`, `AudioDataPlanePtr`
- Boolean getters: `is_` / `has_` prefix — `is_running()`, `has_audio_input()`

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
- Use `std::make_shared` for all `shared_ptr` construction

### Smart Pointers
- `std::shared_ptr` is the primary ownership type; always alias as `FooPtr`
- `std::weak_ptr` for parent/back references to avoid cycles
- Raw pointers only for non-owning references (e.g., RtAudio callback buffers)

### Modern C++ Idioms
- `std::optional<T>` for fallible return values instead of out-parameters or sentinel values
- `std::variant` for type-safe discriminated unions (e.g., `SourceVariant`)
- Range-based `for` loops throughout; avoid raw index loops unless the index is needed
- `alignas(64)` on `std::atomic` members that occupy hot cache lines
- `thread_local` for per-thread state (e.g., thread name in Logger)

### Comments & Documentation
- All public API declarations use Doxygen: `@brief`, `@param`, `@return`, `@throws`, `@deprecated`
- Inline comments only for non-obvious logic; do not comment self-evident code

## Architecture
- Strict layered system: framework (Layer 0) -> data (Layer 1, real-time) -> processing (Layer 2, partial/experimental) -> control (Layer 3, synchronous) -> public/cli/examples (Layer 4).
- Control plane is synchronous singletons (main thread, locks allowed):
	- Controllers in `src/control/` (`AudioStreamController`, `MidiPortController`)
	- Public managers in `src/public/` (`TrackManager`, `DeviceManager`, `FileManager`)
- Data plane executes in RtAudio/RtMidi callbacks in `src/data/` and must be lock-free and allocation-free; use `LockfreeRingBuffer<T, Size>` from `src/framework/include/lockfree_ringbuffer.h` for cross-thread messaging.
- Data plane components: `AudioDataPlane`, `MidiDataPlane`, `AudioCallbackHandler`, `MidiCallbackHandler`.
- Processing plane components are minimal: `IAudioProcessor`, `Sample`, `SamplePlayer` (threading is per-processor via `core::IProcessor`; no orchestration layer yet).

## Build and Test
- Configure: `cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`
- Build: `cmake --build build` (Windows configs can add `--config Debug|Release`)
- Unit tests: Windows `\build\tests\unit\Debug\miniaudioengine-unit-tests.exe`; Linux `./build/tests/unit/miniaudioengine-unit-tests`
- Windows setup details and vcpkg toolchain usage: `WINDOWS_SETUP.md`

## Project Conventions
- Singletons via `::instance()`; unit tests often call `TrackManager::instance().clear_tracks()` to reset state.
- Real-time safety rules for data plane: no mutexes, no allocations, no blocking I/O, keep callback work under 1 ms.
- `MessageQueue<T>` exists in `src/framework/include/messagequeue.h` but is not used for real-time data paths.
- `IAudioController` is deprecated; prefer `AudioStreamController` directly (still referenced in `TrackManager`).

## Integration Points
- External dependencies managed by vcpkg in `vcpkg.json`: RtAudio, RtMidi, libsndfile, gtest.
- Platform detection macros `PLATFORM_WINDOWS` / `PLATFORM_LINUX` are used for OS-specific code paths.

## Security
- External inputs: audio/MIDI devices and file I/O (libsndfile) are system-facing; validate assumptions in control plane, never in real-time callbacks.
- Windows runtime may require vcpkg DLLs on PATH; see `WINDOWS_SETUP.md` troubleshooting.
