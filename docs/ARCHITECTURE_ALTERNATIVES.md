# Architecture Alternatives for miniaudioengine

> Prepared: March 2026  
> Baseline: current layered architecture (v1)

---

## Current Architecture — Summary and Issues

The current design is a five-layer stack:

```
Layer 4: public / cli        TrackManager, DeviceManager, FileManager, CLI
Layer 3: control             AudioStreamController, MidiPortController
Layer 2: processing          IAudioProcessor, Sample, SamplePlayer
Layer 1: data                AudioDataPlane, MidiDataPlane (RT callbacks)
Layer 0: framework           LockfreeRingBuffer, Logger, IController, IDataPlane…
```

### Strengths
- Real-time (RT) callbacks are isolated in `src/data/`
- Lock-free primitives (`LockfreeRingBuffer`, `DoubleBuffer`) are kept at layer 0
- Template-Method pattern gives controlled extension points in `IController` / `IDataPlane`
- Factory pattern on file types (`WavFile`, `MidiFile`) via `FileManager` prevents misuse

### Known Problems

| # | Issue | Detail |
|---|---|---|
| 1 | **Upward dependency violations** | `data-audio` links to `trackmanager` (Layer 1 → Layer 4); `data-midi` links to `control-midi` (Layer 1 → Layer 3) |
| 2 | **Blurred internal / public boundary** | `IAudioController`, `IAudioDevice`, `IDevice`, `IDataPlane` are internal but live in headers that end-users can reach |
| 3 | **Duplicate concrete classes** | `AudioController` / `AudioStreamController` and `MidiController` / `MidiPortController` are near-identical; one of each is deprecated but still compiled |
| 4 | **Namespaces do not track visibility** | `miniaudioengine::core` mixes RT types (`AudioDataPlane`) with pure framework utilities (`LockfreeRingBuffer`) |
| 5 | **No stable ABI boundary** | The public API (`TrackManager`, etc.) exposes STL containers and internal types directly in headers |
| 6 | **Interface headers co-located with concrete headers** | `IAudioController` lives next to `AudioStreamController`; nothing prevents users from including it |
| 7 | **`IManager` is an empty interface** | Adds an inheritance level with no contracts enforced |

---

## Option A — Explicit Public/Internal Namespace Split with Umbrella Header

> **Tagline:** "Same directory layout, radical namespace clarity."  
> **Effort:** Low — mostly renaming and re-organisation, no structural rewrite.

### Core Idea

Introduce a hard **namespace convention** that acts as the access-control boundary:

| Namespace | Meaning | Headers location |
|---|---|---|
| `mae` | Public stable API — everything users should touch | `include/mae/*.h` (single flat include dir) |
| `mae::detail` | Internal implementation — not for users | `src/**` only, never installed |
| `mae::rt` | Real-time hot path — RT-safe contract enforced | `src/data/**` |
| `mae::test` | Test helpers and mocks | `tests/mocks/**` |

The `include/mae/` directory is the **only** directory added to consumer include paths via CMake `INSTALL TARGETS` / `target_include_directories(... PUBLIC ...)`.  Internal headers are `PRIVATE` in CMake and never installed.

### Namespace Mapping

```
mae             TrackManager, DeviceManager, FileManager, CLI
                Track, MainTrack
                AudioDevice, MidiDevice         (value types)
                WavFile, MidiFile               (value types)
                SamplePlayer                    (user-facing processor)

mae::detail     IController, IDataPlane, IDevice, IAudioDevice, IProcessor
                IManager (or remove entirely)
                AudioStreamController, MidiPortController
                AudioCallbackHandler, MidiCallbackHandler
                AudioOutputStatistics, MidiInputStatistics
                LockfreeRingBuffer, DoubleBuffer, MessageQueue

mae::rt         AudioDataPlane, MidiDataPlane   (RT callback objects)
                (separates RT-safety contract from generic "detail")

mae::test       MockController, MockDataPlane, MockDevice, MockAudioController
```

### File/Library Structure

```
include/
  mae/
    track_manager.h       // TrackManager, Track, MainTrack
    device_manager.h      // DeviceManager, AudioDevice, MidiDevice
    file_manager.h        // FileManager, WavFile, MidiFile
    sample_player.h       // SamplePlayer
    cli.h                 // CLI
    mae.h                 // single umbrella: #includes all of the above

src/
  framework/              (mae::detail utilities — LockfreeRingBuffer etc.)
  control/                (mae::detail — AudioStreamController, MidiPortController)
  data/                   (mae::rt    — AudioDataPlane, MidiDataPlane)
  processing/             (mae::detail — IAudioProcessor, Sample, SamplePlayer impl)
  public/                 (mae        — TrackManager, DeviceManager, FileManager, CLI)
```

### CMake Libraries

```
mae-framework             STATIC   (detail internals — never exposed in install)
mae-control               STATIC   (detail internals)
mae-data                  STATIC   (rt internals)
mae-processing            STATIC   (detail internals)
mae                       SHARED/STATIC  PUBLIC target — the only installed target
                          links everything privately
```

### Fixes Layer Violations
- `mae::rt::AudioDataPlane` accepts a pre-built list of tracks (passed in by `TrackManager`) rather than pulling them from a global singleton. This removes the `data-audio → trackmanager` link.
- MIDI data types move to `mae::detail` / `mae::rt` (not `control-midi`) to remove the `data-midi → control-midi` violation.

### Trade-offs

| Pro | Con |
|---|---|
| Minimal structural change | Namespace `mae` is short — possible collision in large projects |
| Rename keeps existing patterns intact | Still no stable ABI (STL in headers) |
| Easy to enforce with clang-tidy `namespace` rules | `mae::detail` is a convention, not enforced by compiler |

---

## Option B — Hexagonal Architecture (Ports and Adapters)

> **Tagline:** "Invert all the dependencies — the domain knows nothing about hardware."  
> **Effort:** Medium-High — significant reorganisation of control and data layers.

### Core Idea

Hardware (RtAudio, RtMidi, libsndfile) lives at the **outermost ring**. The audio engine **domain** is at the centre and defines ports (pure C++ interfaces) that it calls. Hardware adapters plug in from outside. Mirror of the "Clean Architecture" pattern.

```
┌──────────────────────────────────────────────────────┐
│  Outer ring (Adapters)                               │
│  RtAudioAdapter, RtMidiAdapter, SndFileAdapter       │
│  CLI, FileSystem                                     │
├──────────────────────────────────────────────────────┤
│  Application ring (Use cases / Services)             │
│  PlaybackService, RecordingService, DeviceService    │
├──────────────────────────────────────────────────────┤
│  Domain ring (Entities & Ports)                      │
│  Track, TrackGraph, IAudioPort, IMidiPort            │
│  IAudioRenderer, IAudioSource, IMidiSource           │
├──────────────────────────────────────────────────────┤
│  RT core (isolated, no STL allocations)              │
│  AudioEngine (callback), MidiEngine (callback)       │
│  LockfreeRingBuffer, DoubleBuffer                    │
└──────────────────────────────────────────────────────┘
```

### Domain Ports (pure interfaces, no hardware dependencies)

```cpp
// Domain ring — src/domain/include/
namespace mae::domain {

struct AudioBuffer { float* data; uint32_t frames; uint32_t channels; };

class IAudioSource {
public:
    virtual ~IAudioSource() = default;
    virtual bool read(AudioBuffer&) noexcept = 0;     // RT-safe
    virtual bool is_exhausted() const noexcept = 0;
};

class IAudioSink {
public:
    virtual ~IAudioSink() = default;
    virtual void write(const AudioBuffer&) noexcept = 0;
};

class IMidiSource {
public:
    virtual ~IMidiSource() = default;
    // Messages delivered via callback — no virtual call in RT path
};

} // namespace mae::domain
```

### Adapter Ring

```cpp
// Outer ring — src/adapters/
namespace mae::adapters {

// Wraps RtAudio; implements mae::domain::IAudioSink
class RtAudioOutputAdapter : public domain::IAudioSink { ... };

// Wraps RtMidiIn; fires domain MIDI events
class RtMidiInputAdapter { ... };

// Wraps libsndfile; implements mae::domain::IAudioSource
class SndFileAdapter : public domain::IAudioSource { ... };

} // namespace mae::adapters
```

### Namespace Mapping

| Namespace | Ring | Content |
|---|---|---|
| `mae` | Public | `PlaybackService`, `DeviceService`, `FileService`, `CLI` |
| `mae::domain` | Domain | `Track`, `TrackGraph`, `IAudioSource`, `IAudioSink`, `IMidiSource` |
| `mae::rt` | RT core | `AudioEngine`, `MidiEngine`, `LockfreeRingBuffer`, `DoubleBuffer` |
| `mae::adapters` | Adapters | `RtAudioOutputAdapter`, `RtMidiInputAdapter`, `SndFileAdapter` |
| `mae::detail` | Internal utils | `Logger`, `MessageQueue`, `IProcessor` |
| `mae::test` | Test helpers | `MockAudioSource`, `MockAudioSink`, `MockMidiSource` |

### File/Library Structure

```
src/
  domain/               (mae::domain — zero hardware dependencies, easily unit-tested)
    include/
      track.h           Track, TrackGraph
      audio_port.h      IAudioSource, IAudioSink, AudioBuffer
      midi_port.h       IMidiSource, MidiMessage (domain model)
    src/
      track.cpp
      track_graph.cpp

  application/          (mae — use-case services, orchestrate domain + adapters)
    include/
      playback_service.h
      device_service.h
      file_service.h
    src/
      playback_service.cpp

  adapters/             (mae::adapters — all hardware-specific code here)
    rtaudio/            RtAudioOutputAdapter
    rtmidi/             RtMidiInputAdapter
    sndfile/            SndFileAdapter

  rt/                   (mae::rt — RT callback engine, no STL alloc, no locks)
    include/
      audio_engine.h    AudioEngine (owns callback, accepts IAudioSource list)
      midi_engine.h     MidiEngine
      lockfree_ringbuffer.h
      double_buffer.h
    src/
      audio_engine.cpp
      midi_engine.cpp

  framework/            (mae::detail — Logger, MessageQueue)
```

### Interface Locations

All domain interfaces (`IAudioSource`, `IAudioSink`, etc.) live in `src/domain/include/` and have **no header dependencies on RtAudio, libsndfile, or any adapter**.  
All control interfaces (`IAudioController` equivalent) are replaced by `PlaybackService` which operates on domain ports.

### Layer Violation Fix

Because `AudioEngine` (previously `AudioDataPlane`) depends only on `IAudioSource` (domain interface), there is no link to `TrackManager` at all. `PlaybackService` constructs the source list and passes it to `AudioEngine` before starting the callback.

### Trade-offs

| Pro | Con |
|---|---|
| True dependency inversion — domain has zero hardware includes | Significant refactor effort |
| Domain layer is 100% unit-testable without mocks for hardware | Two more rings = more indirection for simple code paths |
| Adapters are swappable (e.g., replace RtAudio with PortAudio entirely) | Team must understand Ports/Adapters mental model |
| `mae::domain` compiles and tests without any external dependencies | More CMake targets to maintain |

---

## Option C — Facade + Pimpl API with Opaque Handle Types

> **Tagline:** "A rock-stable ABI and zero internal type leakage."  
> **Effort:** Medium — rewrite public headers only; internals mostly unchanged.

### Core Idea

Every public type in `include/mae/` uses **Pimpl (pointer to implementation)** so the public header never exposes STL containers, RtAudio types, or internal classes. An opaque `struct TrackImpl;` forward-declaration is all users see. This achieves:
1. Stable ABI across shared-library versions.
2. Zero internal type leakage into consumer translation units.
3. The ability to ship a pre-compiled `mae.dll` / `libmae.so` with only headers.

### Public Header Shape

```cpp
// include/mae/track.h
#pragma once
#include <memory>
#include <cstdint>
#include "mae/types.h"    // DeviceHandle, FileHandle — also opaque

namespace mae {

class Track {
public:
    ~Track();
    Track(Track&&) noexcept;
    Track& operator=(Track&&) noexcept;

    // No STL containers in API — use indices/counts
    void   set_audio_source(DeviceHandle device);
    void   set_audio_source(FileHandle file);
    void   set_gain(float gain) noexcept;
    float  get_gain() const noexcept;
    bool   play();
    bool   stop();
    bool   is_playing() const noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    explicit Track(std::unique_ptr<Impl>);
    friend class TrackManager;  // sole factory
};

using TrackHandle = std::unique_ptr<Track>;

} // namespace mae
```

```cpp
// include/mae/track_manager.h
#pragma once
#include "mae/track.h"

namespace mae {

class TrackManager {
public:
    static TrackManager& instance();
    TrackHandle create_track();
    void        remove_track(TrackHandle&);
    std::size_t track_count() const;
    bool        start_engine();
    bool        stop_engine();
};

} // namespace mae
```

### Namespace Mapping

| Namespace | Visibility | Content |
|---|---|---|
| `mae` | **Public** (installed headers) | `Track`, `TrackManager`, `DeviceManager`, `FileManager`, `CLI`, all handle types |
| `mae::impl` | **Private** (never installed) | All concrete classes — `AudioStreamController`, `AudioDataPlane`, `AudioDevice`, `WavFile`, etc. |
| `mae::rt` | **Private** | `AudioEngine` callback, `LockfreeRingBuffer`, `DoubleBuffer` |
| `mae::test` | **Test only** | Mocks |

The key difference from Option A: **`AudioDevice`, `WavFile`, `MidiFile`, `AudioDevice`** are now `mae::impl` types, not public ones. Users interact with them through `DeviceHandle` / `FileHandle` opaque wrappers.

### File/Library Structure

```
include/
  mae/
    mae.h               // single umbrella include
    track.h             // Pimpl type — all users see
    track_manager.h
    device_manager.h
    file_manager.h
    cli.h
    types.h             // DeviceHandle, FileHandle, enums, error codes

src/
  impl/                 (mae::impl — never installed)
    track_impl.h        Track::Impl definition
    track_impl.cpp
    audio_device.h      concrete AudioDevice (wraps RtAudio::DeviceInfo)
    wav_file.h
    audio_stream_controller.h
    midi_port_controller.h
  rt/                   (mae::rt — real-time)
    audio_engine.h/cpp
    midi_engine.h/cpp
    lockfree_ringbuffer.h
  framework/            (mae::impl — utilities)
    logger.h/cpp
    message_queue.h
```

### CMake Libraries

```cmake
# Public-facing interface target — only this is exported/installed
add_library(mae SHARED ...)
target_include_directories(mae PUBLIC  include/)     # public headers only
target_include_directories(mae PRIVATE src/)         # impl headers private

# impl and rt are OBJECT libraries — dissolved into mae.dll
add_library(mae-impl  OBJECT src/impl/...)
add_library(mae-rt    OBJECT src/rt/...)
add_library(mae-fw    OBJECT src/framework/...)
```

### Interface Placement

Because `IAudioController`, `IMidiController`, `IDataPlane`, `IController` are all `mae::impl` now, they move to `src/impl/interfaces/`:

```
src/impl/interfaces/
  i_audio_controller.h
  i_midi_controller.h
  i_data_plane.h
  i_controller.h
  i_audio_processor.h
```

Tests use the `mae` public API plus `mae::test` mocks. They do **not** need to include `src/impl/`.

### Trade-offs

| Pro | Con |
|---|---|
| True ABI stability — `mae.dll` can be updated without recompiling consumers | `unique_ptr<Impl>` prevents inline operations; slight overhead |
| Zero internal type leakage | More boilerplate (Pimpl forwarding methods) |
| Simplest public include tree imaginable (`#include <mae/mae.h>`) | Harder to debug (step-into goes through forwarding wrapper) |
| Can distribute pre-compiled binary + headers | Move semantics must be explicitly supported per type |

---

## Option D — Component/Plugin Architecture with Service Locator

> **Tagline:** "Runtime composable components; swap RT backends at load time."  
> **Effort:** Medium — refactor around a central registry; no Pimpl required.

### Core Idea

Rather than a rigid class hierarchy, the engine is a **registry of named services**. Each service implements a well-known interface. The `TrackManager` / engine core holds a `ServiceLocator`. This allows:
- Swapping `RtAudioOutputAdapter` for a test `NullAudioBackend` at runtime (no recompile).
- Adding new device types (e.g., ASIO, CoreAudio wrappers) by registering a new service.
- Separating the RT audio engine from the MIDI engine without tight coupling.

```cpp
// mae::services — all injectable interfaces
namespace mae::services {

class IAudioBackend {
public:
    virtual ~IAudioBackend() = default;
    virtual bool open_stream(const AudioConfig&) = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual std::vector<AudioDeviceInfo> enumerate_devices() = 0;
};

class IMidiBackend {
public:
    virtual ~IMidiBackend() = default;
    virtual std::vector<MidiPortInfo> enumerate_ports() = 0;
    virtual bool open_port(uint32_t port_index) = 0;
    virtual void close_port() = 0;
};

class IAudioFileReader {
public:
    virtual ~IAudioFileReader() = default;
    virtual std::optional<AudioFileHandle> open(const std::filesystem::path&) = 0;
};

} // namespace mae::services
```

```cpp
// mae::core — service registry
namespace mae::core {

class ServiceLocator {
public:
    static ServiceLocator& instance();

    template<typename T>
    void register_service(std::shared_ptr<T> impl);

    template<typename T>
    std::shared_ptr<T> get_service() const;
};

} // namespace mae::core
```

### Namespace Mapping

| Namespace | Role | Content |
|---|---|---|
| `mae` | Public API | `TrackManager`, `DeviceManager`, `FileManager`, `Engine`, `CLI` |
| `mae::services` | Service Interfaces (public) | `IAudioBackend`, `IMidiBackend`, `IAudioFileReader`, `IAudioProcessor` |
| `mae::backends` | Default implementations (optional compile) | `RtAudioBackend`, `RtMidiBackend`, `SndFileReader` |
| `mae::core` | Engine core | `ServiceLocator`, `TrackGraph`, `AudioEngine`, `MidiEngine` |
| `mae::rt` | RT primitives | `LockfreeRingBuffer`, `DoubleBuffer`, `AudioBuffer` |
| `mae::test` | Test helpers | `NullAudioBackend`, `RecordingAudioBackend` (for test playback capture) |

### File/Library Structure

```
include/
  mae/
    mae.h               // public umbrella
    track_manager.h
    device_manager.h
    engine.h            // new: Engine class wraps ServiceLocator setup
    services/
      i_audio_backend.h
      i_midi_backend.h
      i_audio_file_reader.h
      i_audio_processor.h

src/
  core/                 (mae::core — ServiceLocator, TrackGraph, AudioEngine, MidiEngine)
  rt/                   (mae::rt — RT primitives, no STL alloc)
  backends/             (mae::backends — RtAudio, RtMidi, SndFile adapters)
    rtaudio/
    rtmidi/
    sndfile/
  public/               (mae — TrackManager, DeviceManager, FileManager, CLI impl)
  framework/            (mae::detail — Logger, MessageQueue)
```

### CMake Libraries

```cmake
mae-rt          STATIC  (RT primitives — no external deps)
mae-core        STATIC  (ServiceLocator, engines — depends on mae-rt)
mae-backends    STATIC  (RtAudio, RtMidi, SndFile adapters — depends on mae-core + externals)
mae             SHARED  (public API — depends on mae-core; optionally links mae-backends)

# Test executable can link mae-core + test null backends, skipping mae-backends entirely
```

### Interface Placement Strategy

Service interfaces (`IAudioBackend`, `IMidiBackend`, etc.) are **public** — in `include/mae/services/`. They define the contract for extension. Concrete control/data classes (`AudioStreamController`, `AudioDataPlane`) become **`mae::backends`** — private implementation of those service interfaces.

Internal framework interfaces (`IController`, `IDataPlane`) either collapse into the new service interfaces or become `mae::core` detail types with **no installed headers**.

### Trade-offs

| Pro | Con |
|---|---|
| Zero static coupling to RtAudio/RtMidi in engine core | Service Locator is a form of global state — order-of-registration bugs possible |
| Test doubles injected via `register_service<IAudioBackend>(make_shared<NullBackend>())` | Template-based registry has complex error messages |
| New backends added without touching engine code | Service discovery at runtime is implicit |
| `mae::services` interfaces are stable and versioned | Slightly more verbose setup (manual service wiring) |

---

## Option E — Module-Based Static Library Split with Strict Include Guards

> **Tagline:** "Keep the familiar OOP design but enforce boundaries through CMake and header organisation."  
> **Effort:** Low-Medium — no design paradigm shift; fix violations and re-partition headers.

### Core Idea

Retain the current class hierarchy and patterns but restructure into **four independently installable static libraries** with hard CMake include-visibility rules. Each library exposes only what its consumers need via `target_include_directories(... PUBLIC ...)` vs `PRIVATE`.

```
mae-rt           (real-time primitives only)
mae-engine       (control + data — depends on mae-rt)
mae-io           (device + file I/O — depends on mae-rt)
mae-api          (public managers + CLI — depends on all above)
```

### Namespace Strategy

The namespace boundary **mirrors the library boundary**:

| Library | Namespace | Visibility |
|---|---|---|
| `mae-rt` | `mae::rt` | Partially public (LockfreeRingBuffer useful to extensions) |
| `mae-engine` | `mae::engine` | Internal (not installed by default) |
| `mae::engine::detail` | Internal only — interfaces | |
| `mae-io` | `mae::io` | Internal |
| `mae-api` | `mae` | Public — installed targets |

### Concrete Interface Separation

The current problem is that `IAudioController`, `IMidiController`, `IDataPlane` etc. are used in both the control layer and in tests, but live adjacent to their concrete implementations. The fix is to split each component into three files:

```
src/engine/audio/
  interfaces/
    i_audio_controller.h    // mae::engine::detail — defines pure virtual interface
  include/                  // internal includes for engine-internal use
    audio_stream_controller.h
  src/
    audio_stream_controller.cpp
```

Only `i_audio_controller.h` is passed as a `PUBLIC` include to targets that need to mock it. The concrete `AudioStreamController` header is never `PUBLIC`.

### Fixing Layer Violations (Option E Approach)

**`data-audio` → `trackmanager` violation:**  
`AudioDataPlane` currently calls `TrackManager::instance()` from inside the RT callback. Replace with a **pre-built snapshot**: `TrackManager` calls `AudioEngine::set_active_tracks(vector<AudioDataPlanePtr>)` before starting the stream. `AudioDataPlane` (RT side) receives only the buffer snapshot — no singleton call.

```cpp
// mae::rt — RT-safe, no singleton lookup
class AudioEngine {
public:
    // Called from control thread before stream start
    void set_active_planes(std::span<AudioDataPlane*> planes) noexcept;
private:
    static int callback(void*, void*, unsigned int, double, RtAudioStreamStatus, void*) noexcept;
    std::array<AudioDataPlane*, kMaxTracks> m_active_planes{};
    std::atomic<size_t> m_active_plane_count{0};
};
```

**`data-midi` → `control-midi` violation:**  
Move `MidiPort`, `MidiMessage`, `eMidiMessageType` (currently in `control/midi/miditypes.h`) to `mae-rt` or a shared `mae::types` header. Neither `data-midi` nor `control-midi` then depends on the other.

### File/Library Structure

```
include/
  mae/
    track_manager.h     // mae — public
    device_manager.h
    file_manager.h
    cli.h
    types.h             // mae::types — AudioDeviceInfo, MidiPortInfo, MidiMessage (moved here)

src/
  rt/                   (mae::rt)
    lockfree_ringbuffer.h
    double_buffer.h
    audio_engine.h / .cpp
    midi_engine.h / .cpp
    audio_data_plane.h / .cpp
    midi_data_plane.h / .cpp

  engine/               (mae::engine — control layer internals)
    interfaces/
      i_audio_controller.h    (PUBLIC to mocks, PRIVATE to users)
      i_midi_controller.h
      i_audio_processor.h
    audio/
      audio_stream_controller.h / .cpp
    midi/
      midi_port_controller.h / .cpp
    processing/
      sample.h / .cpp
      sample_player.h / .cpp

  io/                   (mae::io — device + file managers internal impl)
    audio_device.h / .cpp
    midi_device.h / .cpp
    wav_file.h / .cpp
    midi_file.h / .cpp

  public/               (mae — public manager implementations)
    track_manager.cpp
    device_manager.cpp
    file_manager.cpp
    cli.cpp

  framework/            (mae::detail — Logger, MessageQueue, realtime_assert)
```

### CMake Structure

```cmake
# mae-rt: RT primitives — used by engine and by tests
add_library(mae-rt STATIC src/rt/audio_engine.cpp src/rt/midi_engine.cpp ...)
target_include_directories(mae-rt PUBLIC  src/rt/)      # LockfreeRingBuffer exposed
target_include_directories(mae-rt PRIVATE ...)

# mae-engine: control layer — internal interfaces are PUBLIC for mocking
add_library(mae-engine STATIC src/engine/audio/... src/engine/midi/... ...)
target_include_directories(mae-engine
    PUBLIC  src/engine/interfaces/   # IAudioController etc. visible to test-mocks
    PRIVATE src/engine/audio/        # concrete controllers NOT visible externally
            src/engine/midi/
)
target_link_libraries(mae-engine PUBLIC mae-rt)

# mae-io: file and device I/O
add_library(mae-io STATIC src/io/...)
target_include_directories(mae-io PRIVATE src/io/)   # AudioDevice etc. never exposed
target_link_libraries(mae-io PRIVATE sndfile mae-rt)

# mae-api: public managers — final assembly
add_library(mae-api STATIC src/public/...)
target_include_directories(mae-api PUBLIC include/mae/)  # THE public include path
target_link_libraries(mae-api PRIVATE mae-engine mae-io mae-rt)

# Tests: use mae-api public API + mae-engine interfaces for mocking
target_link_libraries(unit-tests PRIVATE mae-api mae-engine test-mocks GTest::gtest)
```

### Trade-offs

| Pro | Con |
|---|---|
| Minimal paradigm shift — class hierarchy unchanged | Does not address ABI stability |
| CMake `PRIVATE` include dirs enforce the boundary mechanically | Requires careful auditing of all `target_include_directories` |
| Layer violations fixed without architectural overhaul | `mae::engine::detail` namespace is still a convention |
| Interface headers cleanly separated from concrete headers | More CMake targets to maintain |

---

## Comparison Matrix

| Criterion | Option A (NS Split) | Option B (Hexagonal) | Option C (Pimpl API) | Option D (Plugin/SvcLoc) | Option E (Module Split) |
|---|---|---|---|---|---|
| **Effort** | Low | High | Medium | Medium | Medium |
| **Fixes layer violations** | Yes | Yes | Yes | Yes | Yes |
| **ABI stability** | No | No | **Yes** | No | No |
| **Internal type leakage** | Partial | No | **None** | Partial | Partial |
| **Hardware swappable** | No | **Yes** | No | **Yes** | No |
| **RT isolation** | Namespace only | Structural | Namespace only | Structural | Structural |
| **Test ergonomics** | Good | Excellent | Good | Excellent | Good |
| **Paradigm shift** | Minimal | Large | Moderate | Moderate | Minimal |
| **Duplicate class removal** | Natural | Natural | Natural | Natural | Forced |

---

## Recommended Path

For this project's maturity level and team size, a **two-phase approach** is practical:

### Phase 1 — Option E (Quick Win)
Apply the module-split and layer-violation fixes immediately. This gives:
- Broken upward dependencies fixed without a paradigm shift.
- Interface headers separated from concrete headers via CMake `PRIVATE` includes.
- `mae::rt` namespace for real-time types.
- Duplicate classes (`AudioController`, `MidiController`) removed.

### Phase 2 — Selective Option C or B
Once the layer structure is clean, choose one of:
- **Option C (Pimpl)** if the goal is distributing a stable SDK/library that third parties link against.
- **Option B (Hexagonal)** if the goal is making the domain logic unit-testable without any audio hardware and supporting multiple backends (ASIO, CoreAudio, JACK).

These two are not mutually exclusive: the public API layer (Option C Pimpl) can sit *on top of* a hexagonal inner core (Option B).

---

## Quick Namespace Reference (All Options)

```
Option A:   mae  |  mae::detail  |  mae::rt  |  mae::test
Option B:   mae  |  mae::domain  |  mae::rt  |  mae::adapters  |  mae::detail  |  mae::test
Option C:   mae  |  mae::impl    |  mae::rt  |  mae::test
Option D:   mae  |  mae::services|  mae::backends  |  mae::core  |  mae::rt  |  mae::test
Option E:   mae  |  mae::engine  |  mae::io  |  mae::rt  |  mae::detail  |  mae::test
```

All options share the convention: **`mae` (no suffix) = public stable API; suffixed subnamespaces = internal/specialised.**
