# Project Guidelines

## Code Style
- C++20 required; keep layer-specific namespaces aligned with the current codebase:
	- `miniaudioengine::core` for framework primitives and data planes
	- `miniaudioengine::audio` for audio control/processing types
	- `miniaudioengine::midi` for MIDI control types
	- `miniaudioengine` (root) for public API managers and CLI
- Public headers in `include/<component>.h`, implementations in `src/<component>.cpp`, include guards use `#ifndef __COMPONENT_H__` style.
- Logging via `LOG_INFO()`, `LOG_WARNING()`, `LOG_ERROR()`, `LOG_DEBUG()` in `src/framework/include/logger.h`.

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
