# Mini Audio Engine (C++)

# Introduction

`miniaudioengine` is a cross-platform, lightweight, audio processing C++ SDK. The intention is for audio applications written using this SDK to run on platforms as minimal as a Raspberry Pi.

---

# Requirements

## Functional Requirements

- Monitor **audio device** inputs.
- Read **WAV audio files**.
- Monitor **MIDI device** inputs and process **MIDI messages**.
- Read **MIDI files** and process MIDI messages.
- Playback to **audio output device**.
- Write to **WAV audio file**.
- Create **processing** components in between I/O.

## Non-Functional Requirements

- Real-time processing.
- Efficient resource management.
- Cross-platform on **x86** and **ARM64** architectures.
- Modern, C++ code following best practices.

---

# Background

## VST Audio Plugins

## Digital Audio Workstation

---

# High-Level Design

## System Architecture

![alt text](docs/system_architecture.png)

## User Interface

### SDK

**I/O**

- Device Manager
- File Manager
- Audio Device
- MIDI Device
- WAV File
- MIDI File

**Tracks**

- Track Manager
- Track

## Control Layer

## Data Layer

## Processing Layer

## Framework/External Layer

---

# Software Implementation

## Project Structure

```
examples/
    sampler/
    wav-file-player/
    midi-controller-interface/
src/
    public/
        trackmanager/
        cli/
        io/
            devicemanager/
            filemanager/
    framework/
    control/
        audio/
        midi/
    data/
        audio/
        midi/
    processing/
    cli/
tests/
    mocks/
    unit/
samples/
cmake/
docker/
docs/
CMakeLists.txt
Dockerfile
vcpkg.json
```

## Build Environment

### Developing on Windows

**Requirements**

- Visual Studio 2022 or later (with C++20 support)
- CMake 3.25+
- vcpkg


**Build Steps**

```bash
# Configure with vcpkg integration
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
cmake --build build
```

### Developing on Linux

**Requirements**

- GCC 11+ or Clang 14+ (with C++20 support)
- CMake 3.25+
- vcpkg or system packages (RtAudio, RtMidi, libsndfile)

**Build Steps**

```bash
# Configure
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
cmake --build build
```

### Docker

For reproducible Linux builds across **x86_64** and **ARM64**

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

## Example Programs

### WAV File Player

1. Set up resource managers

```cpp
TrackManager &track_manager = TrackManager::instance();
DeviceManager &device_manager = DeviceManager::instance();
FileManager &file_manager = FileManager::instance();
```

1. Add a Track to the Track Manager

```cpp
size_t track_id = track_manager.add_track();
auto track = track_manager.get_track(track_id);
```

1. Set audio output device

```cpp
auto output_device = device_manager.get_audio_device(audio_output_device_id);

track_manager.set_audio_output_device(output_device);
```

1. Open WAV file

```cpp
auto wav_file = file_manager.read_wav_file(input_file_path);

track->add_audio_input(wav_file);
```

1. Set Track event callback for end of playback

```cpp
track->set_event_callback([](eTrackEvent event) {
    if (event == eTrackEvent::PlaybackFinished) {
        LOG_INFO("Track playback finished.");
        running = false;
    }
});
```

1. Start WAV file playback

```cpp
track->play();
```

1. Wait until Track playback has finished

```cpp
while (running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
```

1. Get Track statistics

```cpp
auto stats = track->get_statistics();
LOG_INFO("Playback statistics:\n", stats.to_string());
```

### MIDI Controller Interface

### Sampler

## C++ Coding Conventions

### **Naming Conventions**

| Element | Convention | Example |
| --- | --- | --- |
| Classes | PascalCase | `AudioDataPlane`, `FileManager` |
| Interfaces | `I` prefix + PascalCase | `IController`, `IDataPlane` |
| Methods | snake_case | `get_output_device()`, `is_running()` |
| Data members | `m_` prefix | `m_running`, `m_stream_state` |
| Pointer members | `p_` prefix | `p_device`, `p_statistics` |
| Enums | `e` prefix + PascalCase type, PascalCase values | `eStreamState::Playing` |
| `shared_ptr` aliases | `Ptr` suffix | `IDataPlanePtr`, `AudioDataPlanePtr` |

### **Specifiers & Attributes**

- `override` used consistently on all derived virtual methods
- `noexcept` on real-time audio callbacks and time-critical methods
- `explicit` on single-parameter constructors
- `const` extensively applied to getters and parameters
- `constexpr` for compile-time constants/mappings

### **Smart Pointers**

- `std::shared_ptr` is the primary ownership mechanism
- `std::weak_ptr` for parent references (avoiding cycles)
- Raw pointers only for non-owning references (e.g., RtAudio callback buffers)
- `std::make_shared` used for allocation

### **Singletons**

- Thread-safe Meyers singleton via `static T& instance()` with private constructor and deleted copy

### **Comments**

- Doxygen format (`@brief`, `@param`, `@return`, `@throws`, `@deprecated`) on all public APIs
- Brief inline comments for non-obvious logic only

---

# Testing

## Unit Testing

## Profiling / Real-Time Testing

---

# Conclusion

---