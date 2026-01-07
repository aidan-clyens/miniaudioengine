## 1. Introduction
minimal-audio-engine is a cross-platform, cross-architecture, modern C++20 audio processing engine designed for embedded and desktop environments. It emphasizes thread-safe programming, concurrency patterns, real-time audio safety, and modern C++ best practices. The engine implements a strict 3-plane architecture separating control operations, background processing, and real-time data handling.

This project can be used as a foundation for building digital audio workstations, audio plugins (VST), embedded audio platforms, or any application requiring professional-grade audio processing with deterministic real-time performance.

The design focuses on lock-free data structures for real-time contexts, clear separation of concerns across architectural layers, and a hierarchical track routing system for flexible audio mixing and signal flow.

---

## 2. Objectives
- Deliver a **minimal yet extensible audio engine** with strict real-time guarantees suitable for embedded and desktop environments
- Ensure **cross-platform support** across Linux and Windows
- Support **multi-architecture builds** (x86_64, ARM64)
- Implement **lock-free data structures** for real-time audio/MIDI processing
- Provide **hierarchical track routing** with parent-child mixing relationships
- Ensure **thread safety** across control plane, processing plane, and data plane
- Provide a **Dockerized development environment** for reproducibility
- Implement **continuous integration (CI/CD)** for automated builds, testing, and release packaging

---

## 3. System Overview
minimal-audio-engine implements a **3-Plane Architecture** with clear separation of concerns:

### 3.1 Control Plane (Layer 3)
**Synchronous operations called from main thread** - locks allowed, infrequent calls
- **AudioStreamController**: Audio device management (start/stop streams, device selection)
- **MidiPortController**: MIDI port management (open/close ports)
- **TrackManager**: Track lifecycle and hierarchical routing
- **DeviceManager**: Hardware enumeration (audio/MIDI devices)
- **FileManager**: Disk I/O operations (WAV/MIDI file reading/writing via libsndfile)

### 3.2 Data Plane (Layer 1)
**Real-time callback handlers** - lock-free only, < 1ms execution time
- **TrackAudioDataPlane**: Per-track audio rendering in RtAudio callback thread
- **TrackMidiDataPlane**: Per-track MIDI message processing in RtMidi callback thread
- **AudioCallbackHandler**: Pure RtAudio callback function wrapper
- **MidiCallbackHandler**: Pure RtMidi callback function wrapper

### 3.3 Processing Plane (Layer 2)
**Background worker threads for CPU-intensive DSP** - NOT YET IMPLEMENTED

### 3.4 Framework (Layer 0)
**Lock-free primitives and utilities**
- **LockfreeRingBuffer**: Single-producer single-consumer (SPSC) lock-free queue
- **DoubleBuffer**: Atomic double-buffer for producer/consumer patterns
- **Logger**: Thread-safe logging with file and console output
- **MessageQueue**: Thread-safe message passing (legacy pattern for workers)
- **Observer/Subject**: Generic observer pattern implementation

### 3.5 Track Hierarchy
Tracks form a tree structure with **MainTrack** as root:
- Each track can have multiple child tracks
- Audio output routes from children to parent (mixing tree)
- Supports audio input from devices or WAV files
- Supports MIDI input from devices or MIDI files

---

## 4. Build Environment
### 4.1 Dependencies
Managed via **vcpkg**:
- **rtaudio**: Cross-platform audio I/O
- **rtmidi**: Cross-platform MIDI I/O
- **libsndfile**: Audio file read/write (WAV, FLAC, etc.)
- **gtest**: Unit testing framework

### 4.2 Setup for Windows
Requirements:
- Visual Studio 2022 or later (with C++20 support)
- CMake 3.25+
- vcpkg

Build steps:
```bash
# Configure with vcpkg integration
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
cmake --build build

# Run tests
.\build\tests\unit\Debug\minimal-audio-engine-unit-tests.exe
```

See [WINDOWS_SETUP.md](WINDOWS_SETUP.md) for detailed instructions.

### 4.3 Setup for Linux
Requirements:
- GCC 11+ or Clang 14+ (with C++20 support)
- CMake 3.25+
- vcpkg or system packages (RtAudio, RtMidi, libsndfile)

Build steps:
```bash
# Configure
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
cmake --build build

# Run tests
./build/tests/unit/minimal-audio-engine-unit-tests
```

### 4.4 Docker Development Environment
For reproducible Linux builds across x86_64 and ARM64:
```bash
# Build multi-arch Docker image
cd docker
./docker-build.sh

# Run interactive container
./docker-run.sh

# Execute build inside container
cmake -S . -B build
cmake --build build
```

See [Dockerfile](Dockerfile) and [docker/](docker/) scripts for details.

### 4.5 VS Code Tasks
Use the provided tasks for common operations:
- **CMake Configure**: Initialize build system
- **CMake Build**: Compile all targets
- **Run Unit Tests**: Execute test suite
- **Run**: Launch example applications

---

## 5. Architecture
### 5.1 High-Level Design
```
+--------------------------------------+
|       Application Layer (Layer 4)   |
|            CLI / Examples            |
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|      Control Plane (Layer 3)        |
|  AudioStreamController, TrackManager |
|  MidiPortController, DeviceManager,  |
|  FileManager (Synchronous, Main Thread)|
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|    Processing Plane (Layer 2)       |
|   Background DSP Workers             |
|   (NOT YET IMPLEMENTED)              |
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|       Data Plane (Layer 1)          |
|  TrackAudioDataPlane, AudioCallback  |
|  TrackMidiDataPlane, MidiCallback    |
|  (Lock-free, Real-Time Callbacks)    |
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|      Framework (Layer 0)            |
|  LockfreeRingBuffer, DoubleBuffer    |
|  Logger, MessageQueue, Observer      |
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|   Platform Abstraction              |
|  RtAudio, RtMidi, libsndfile         |
+--------------------------------------+
                  |
                  v
+--------------------------------------+
|    Hardware Layer                   |
| Audio Devices, MIDI Devices (x86/ARM)|
+--------------------------------------+
```

### 5.2 Project Structure
```
minimal-audio-engine/
├── src/                          # Source code
│   ├── framework/                # Layer 0 - Lock-free primitives
│   │   └── include/
│   │       ├── engine.h          # Legacy threading pattern (IEngine<T>)
│   │       ├── messagequeue.h    # Thread-safe message queue
│   │       ├── lockfree_ringbuffer.h  # Lock-free SPSC queue
│   │       ├── doublebuffer.h    # Atomic double-buffer
│   │       ├── realtime_assert.h # RT safety macros (stubs)
│   │       ├── logger.h          # Thread-safe logging
│   │       └── observer.h        # Observer pattern
│   ├── dataplane/                # Layer 1 - Real-time components
│   │   ├── audio/
│   │   │   ├── audiodataplane.h         # Per-track audio rendering
│   │   │   └── audiocallbackhandler.h   # RtAudio callback wrapper
│   │   └── midi/
│   │       ├── mididataplane.h          # Per-track MIDI processing
│   │       ├── midicallbackhandler.h    # RtMidi callback wrapper
│   │       └── midicontroltypes.h       # MIDI CC definitions
│   ├── controlplane/             # Layer 3 - Synchronous control
│   │   ├── audio/                # AudioStreamController
│   │   ├── midi/                 # MidiPortController, MIDI types
│   │   ├── trackmanager/         # Track, TrackManager, MainTrack
│   │   ├── devicemanager/        # Device enumeration
│   │   └── filemanager/          # WAV/MIDI file I/O
│   └── cli/                      # Layer 4 - Basic CLI
├── tests/                        # Test suite
│   ├── unit/                     # Unit tests (GTest)
│   ├── profiling/                # Performance profiling
│   └── integration/              # Integration tests (planned)
├── examples/                     # Example applications
│   ├── wav-audio-player/         # WAV file playback example
│   └── midi-device-input/        # MIDI device input example
├── samples/                      # Sample audio/MIDI files
├── docker/                       # Docker build scripts
├── .github/workflows/            # CI/CD pipeline
├── CMakeLists.txt                # Build configuration
├── vcpkg.json                    # Dependency manifest
└── README.md                     # This file
```

### 5.3 Threading Model

#### Control Plane (Main Thread)
- Synchronous singleton operations: `AudioStreamController::instance()`, `TrackManager::instance()`
- Methods block until complete (e.g., `start_stream()`, `stop_stream()`)
- Can use `std::mutex` for thread safety where needed

#### Data Plane (Real-Time Callbacks)
- Audio callback executes in RtAudio's callback thread
- MIDI callback executes in RtMidi's callback thread
- **Strict lock-free requirement**: No mutexes, no allocations, no blocking I/O
- Must complete in < 1ms to avoid dropouts
- Use `LockfreeRingBuffer` for inter-thread communication

#### Framework Lock-Free Primitives
- **LockfreeRingBuffer<T, Size>**: SPSC queue with `memory_order_acquire/release`
- **DoubleBuffer<T>**: Atomic swap for producer/consumer patterns
- Used for passing MIDI events, audio buffers between threads

### 5.4 Key Design Patterns

#### Hierarchical Track Routing
```
MainTrack (root, hardware output)
├── Track 1 (virtual output → MainTrack)
│   ├── Track 1A (virtual output → Track 1)
│   └── Track 1B (virtual output → Track 1)
└── Track 2 (virtual output → MainTrack)
```
- Tracks form tree structure with MainTrack as root
- Each track mixes child outputs into its own output
- Parent-child relationships managed in control plane
- Actual mixing happens in data plane (audio callback)

#### Real-Time Safety
- Data plane: Lock-free only, no allocations
- Use `RT_ASSERT_NO_LOCKS()`, `RT_ASSERT_NO_ALLOCATIONS()` (stubs for now)
- Preload audio data in control plane before playback
- Use lock-free queues for control→data communication

---

## 6. Usage Examples

### 6.1 Playing a WAV File
```cpp
#include "trackmanager.h"
#include "devicemanager.h"
#include "filemanager.h"
#include "audiostreamcontroller.h"

using namespace MinimalAudioEngine::Control;

// Get singleton instances
auto& track_manager = TrackManager::instance();
auto& device_manager = DeviceManager::instance();
auto& file_manager = FileManager::instance();
auto& audio_controller = AudioStreamController::instance();

// Set audio output device on MainTrack
auto output_device = device_manager.get_default_audio_output_device();
audio_controller.set_output_device(output_device.value());

// Create a track and add WAV file input
auto track = track_manager.create_child_track(); // Creates child of MainTrack
auto wav_file = file_manager.read_wav_file("path/to/audio.wav");
track->add_audio_input(wav_file.value());

// Start playback (non-blocking)
track->play();

// Wait for playback to finish...
std::this_thread::sleep_for(std::chrono::seconds(10));

// Stop playback (blocking)
track->stop();

// Get statistics
auto stats = track->get_statistics();
std::cout << stats.to_string() << std::endl;
```

### 6.2 Processing MIDI Input
```cpp
#include "trackmanager.h"
#include "devicemanager.h"

using namespace MinimalAudioEngine::Control;

// Get default MIDI input device
auto midi_device = DeviceManager::instance().get_default_midi_input_device();

// Create track and add MIDI input
auto track = TrackManager::instance().create_child_track();
track->add_midi_input(midi_device.value());

// Set MIDI callbacks
track->set_midi_note_on_callback([](const MidiNoteMessage& msg, TrackPtr track) {
    std::cout << "Note On: " << msg.note_number() 
              << " Velocity: " << msg.velocity() << std::endl;
});

track->set_midi_control_change_callback([](const MidiControlMessage& msg, TrackPtr track) {
    std::cout << "CC: " << msg.controller_number() 
              << " Value: " << msg.controller_value() << std::endl;
});

// Start listening
track->play();
```

### 6.3 Hierarchical Track Routing
```cpp
using namespace MinimalAudioEngine::Control;

auto& track_manager = TrackManager::instance();

// MainTrack is the root (hardware audio output)
auto main_track = track_manager.get_main_track();

// Create parent track
auto parent_track = track_manager.create_child_track(); // Child of MainTrack
parent_track->set_output_gain(0.8f); // 80% volume to MainTrack

// Create child tracks
auto child_track_1 = track_manager.create_child_track(parent_track);
child_track_1->set_output_gain(0.5f); // 50% volume to parent

auto child_track_2 = track_manager.create_child_track(parent_track);
child_track_2->set_output_gain(0.7f); // 70% volume to parent

// Add audio inputs to child tracks
auto wav1 = FileManager::instance().read_wav_file("track1.wav");
auto wav2 = FileManager::instance().read_wav_file("track2.wav");

child_track_1->add_audio_input(wav1.value());
child_track_2->add_audio_input(wav2.value());

// Play both (outputs mix to parent, then to MainTrack hardware output)
child_track_1->play();
child_track_2->play();
```

---

## 7. Testing

### 7.1 Unit Tests
Comprehensive unit tests using Google Test framework:
```bash
# Run all unit tests
cmake --build build
.\build\tests\unit\Debug\minimal-audio-engine-unit-tests.exe
```

Tested components:
- AudioStreamController
- DeviceManager  
- FileManager (WAV file I/O)
- TrackManager (hierarchy, lifecycle)
- Track (audio/MIDI input, playback)
- TrackAudioDataPlane (audio rendering)
- LockfreeRingBuffer (SPSC queue)
- DoubleBuffer (atomic swap)
- MessageQueue (thread-safe queue)
- Observer/Subject pattern

### 7.2 Profiling Tests
Performance tests for lock-free structures and audio processing:
- `test_lockfree_ringbuffer_profiling.cpp`: SPSC throughput at various sample rates
- `test_track_wav_file_profiling.cpp`: WAV file playback performance
- Python analysis scripts for results visualization

### 7.3 Integration Tests
Planned - directory exists but not yet populated.

---

## 8. Continuous Integration (CI/CD)
The repository uses **GitHub Actions** to automate:
1. Multi-arch Docker image builds (linux/amd64, linux/arm64)
2. Library and example builds across Windows and Linux
3. Unit test execution
4. Packaging of build artifacts (`.tar.gz`)
5. Release management

---

## 9. Current Status & Roadmap

### ✅ Completed
- Control Plane: AudioStreamController, MidiPortController, TrackManager, DeviceManager, FileManager
- Data Plane: TrackAudioDataPlane, TrackMidiDataPlane, callback handlers
- Framework: Lock-free primitives (LockfreeRingBuffer, DoubleBuffer), Logger
- Hierarchical track routing with parent-child mixing
- WAV file playback with statistics
- MIDI device input/output
- Unit tests for all major components
- Profiling tests for performance analysis
- Basic CLI implementation
- Example applications (wav-audio-player, midi-device-input)

### 🚧 In Progress
- Processing Plane (Layer 2) for background DSP workers
- RealtimeAssert implementation (stubs exist)
- Refactoring Track to be control-plane only (currently uses Observer pattern and mutex)

### 📋 Planned
- Complete CLI application
- VST3 plugin wrapper
- Additional DSP effects and processors
- MIDI file playback
- Audio recording functionality
- Plugin hosting architecture
- Integration tests

---

## 10. Contributing
This is a personal learning project focused on mastering:
- Modern C++20 features and best practices
- Lock-free data structures and concurrent programming
- Real-time audio processing constraints
- Cross-platform development
- Architectural design patterns

---

## 11. License
See [LICENSE](LICENSE) file for details.

---

## 12. References
- [RtAudio](https://github.com/thestk/rtaudio) - Cross-platform audio I/O library
- [RtMidi](https://github.com/thestk/rtmidi) - Cross-platform MIDI I/O library  
- [libsndfile](https://github.com/libsndfile/libsndfile) - Audio file I/O library
- [Google Test](https://github.com/google/googletest) - C++ testing framework

