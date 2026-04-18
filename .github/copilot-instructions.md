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

See [docs/DESIGN_DOC.md](docs/DESIGN_DOC.md) for full rationale and sequence diagrams.

- Strict layered system — dependencies flow **upward only** (lower layers must never depend on upper layers):

  ```
  Layer 4: services / cli  (TrackService, DeviceService, FileService, CLI)
  Layer 3: engine          (AudioController, AudioDataPlane, MidiController, MidiDataPlane)
  Layer 2: processing      (IAudioProcessor, Sample, SamplePlayer — partial/experimental)
  Layer 1: adapters        (AudioAdapter, FileAdapter, MidiAdapter — PImpl wrappers for RtAudio/RtMidi/libsndfile)
  ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
  core (shared — accessible by all layers):
       LockfreeRingBuffer, DoubleBuffer, Logger, interfaces, DeviceHandle, FileHandle
  ```

- **Track hierarchy**: `MainTrack` is always the root; regular `Track` objects are its direct children (single-level). Audio output mixes upward from children to parent.

  ```
  TrackService (singleton manager)
  └── MainTrack (root — owns AudioController, MidiController)
      ├── Track (owns AudioDataPlane + MidiDataPlane; source = DeviceHandle or FileHandle via std::variant)
      └── Track
  ```

- Service layer is synchronous singletons (main thread, locks allowed):
	- `src/services/` — `TrackService`, `DeviceService`, `FileService`
	- `src/cli/` — CLI entry point
- Engine layer executes in RtAudio/RtMidi callbacks in `src/engine/` — use `LockfreeRingBuffer<T, Size>` from `src/core/` for cross-thread messaging.
- Adapter layer in `src/adapters/` wraps all external library calls; never access RtAudio/RtMidi/libsndfile directly above this layer.
- Processing plane is minimal: `IAudioProcessor`, `Sample`, `SamplePlayer` (per-processor threading via `core::IProcessor`; no orchestration layer yet).

## Build and Test
- Configure: `cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON`
  - **Windows**: append `-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake` if vcpkg is not integrated globally
- Build: `cmake --build build` (Windows: add `--config Debug` or `--config Release`)
- Unit tests:
  - Windows: `.\build\tests\unit\Debug\miniaudioengine-unit-tests.exe`
  - Linux: `./build/tests/unit/miniaudioengine-unit-tests`
  - Run a single suite or test: append `--gtest_filter="TrackManager.*"` or `--gtest_filter="ClassName.MethodName"`
  - Alternative: `ctest -C Debug --output-on-failure` from the `build/` directory
- Windows runtime: vcpkg DLLs must be on `PATH` or copied next to the executable; see [docs/WINDOWS_SETUP.md](docs/WINDOWS_SETUP.md) for full setup.

## Project Conventions
- Singletons via `::instance()`; unit tests often call `TrackService::instance().clear_tracks()` to reset state.
- **Public types**: Use `DeviceHandle` / `DeviceHandlePtr` and `FileHandle` / `FileHandlePtr` as the canonical public API types. `AudioDevice`, `MidiDevice`, `WavFile`, `MidiFile` in `include/miniaudioengine/` are deprecated stubs kept for backward compatibility.
- **Real-time safety rules** (any code in `src/engine/` callbacks):
  1. No mutexes — no `std::mutex`, `std::lock_guard`, or any blocking primitive
  2. No heap allocation — no `new`, `malloc`, `std::vector::push_back`, or dynamic allocation
  3. No blocking I/O — no file reads, no sleep calls
  4. Use `LockfreeRingBuffer` for all cross-thread communication
  5. Keep total callback work under 1 ms
- `MessageQueue<T>` in `src/core/` is lock-based — never use it in real-time paths.
- Mocks live in `tests/mocks/include/` under namespace `miniaudioengine::test`; mirror interface names with `Mock` prefix (e.g., `MockAudioController`).

## Integration Points
- External dependencies managed by vcpkg in `vcpkg.json`: RtAudio, RtMidi, libsndfile, gtest.
- All external library calls are isolated in `src/adapters/` (`AudioAdapter`, `FileAdapter`, `MidiAdapter`); no layer above adapters may include RtAudio, RtMidi, or libsndfile headers directly.
- Platform detection macros `PLATFORM_WINDOWS` / `PLATFORM_LINUX` are used for OS-specific code paths.

## Security
- External inputs: audio/MIDI devices and file I/O (libsndfile) are system-facing; validate assumptions in control plane, never in real-time callbacks.
- Windows runtime may require vcpkg DLLs on PATH; see `WINDOWS_SETUP.md` troubleshooting.
