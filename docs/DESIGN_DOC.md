---
title: "Software Design Description"
subtitle: "miniaudioengine C++ SDK"
author: "Aidan Clyens"
titlepage: true
---

\newpage

# 1. Introduction

\newpage

# 2. Stakeholders and Design Concerns

## 2.1. Stakeholders

| Stakeholder | Responsibilities | Design Concerns |
| -- | -- | -- |
| Software User | - Monitor audio input device.\newline- Monitor MIDI input device.\newline- Read audio files\newline- Play audio to output device.\newline- Play MIDI to output device. | DC-01\newline DC-02\newline DC-03\newline DC-04\newline DC-05\newline DC-06 |
| Third-Party Developer | - Include SDK in C++ software application.\newline- Handle incoming MIDI messages.\newline- Process audio input.\newline- Route processed audio to output device.\newline- Manage audio in multiple tracks. | DC-07\newline DC-08\newline DC-09\newline DC-10\newline DC-11\newline DC-12\newline DC-13\newline DC-14\newline DC-17\newline DC-18 |
| Maintainer | - Maintain CI/CD pipeline.\newline- Manage code repo.\newline- Manage software releases. | DC-13\newline DC-14\newline DC-15\newline DC-16\newline DC-17 |
| Hardware | - Run on a Windows desktop.\newline- Run on an embedded Linux platform. | DC-13\newline DC-14 |

## 2.2. Design Concerns

| ID | Description | Relevant Views |
| -- | -- | -- |
| **DC-01** | Monitor audio input device. | Logical
| **DC-02** | Monitor MIDI input device. | Logical
| **DC-03** | Open and read WAV audio files. | Logical
| **DC-04** | Open and read MIDI files. | Logical
| **DC-05** | Route audio to output device. | Logical
| **DC-06** | Route MIDI to output device. | Logical
| **DC-07** | Processing incoming MIDI messages. | Logical
| **DC-08** | Processing incoming audio streams. | Logical
| **DC-09** | Manage multiple audio tracks. | Logical
| **DC-10** | Add one audio or MIDI input to a track. | Logical
| **DC-11** | Attach one audio or MIDI output to a track. | Logical
| **DC-12** | Chain multiple audio processors in one track. | Logical
| **DC-13** | Build software SDK on Windows and Linux. | Context
| **DC-14** | Build software SDK on x86_64 and ARM64 platforms. |
| **DC-15** | CI/CD pipeline builds and packages software on all compatible platforms. |
| **DC-16** | Complete unit testing and code coverage. |
| **DC-17** | Package software as an SDK used by third-party software. | Context, Composition
| **DC-18** | Third-party software developers manage audio tracks, system audio/MIDI devices, and filesystem. | Composition

\newpage

# 3. Design Views

## 3.1. Context View

Describes the software in context with its external environment. Define users, external components, and the interactions between.

| Design Concern | |
| -- | -- |
| **DC-13** | Build software SDK on Windows and Linux. |
| **DC-17** | Package software as an SDK used by third-party software. |

```mermaid
graph TD
    App["App"]
    SDK["miniaudioengine SDK"]

    subgraph Libraries[External Libraries]
        RtA["RtAudio"]
        RtM["RtMidi"]
        SND["libsndfile"]
    end

    subgraph HW[Hardware / OS]
        AudioHW["Audio Hardware"]
        MIDIHW["MIDI Hardware"]
        FS["File System"]
    end

    App -->|includes| SDK
    SDK --> RtA
    SDK --> RtM
    SDK --> SND
    RtA --> AudioHW
    RtM --> MIDIHW
    SND --> FS
```

\newpage

## 3.2. Composition View

Describe the composition of the **miniaudioengine** SDK software libraries.

| Design Concern | |
| -- | -- |
| **DC-17** | Package software as an SDK used by third-party software. |
| **DC-18** | Third-party software developers manage audio tracks, system audio/MIDI devices, and filesystem. |

```mermaid
graph TD
    subgraph SDK["miniaudioengine SDK"]
        AudioSession
        subgraph deviceService["DeviceService"]
            DeviceService
            Device
        end
        subgraph fileService["FileService"]
            FileService
            File
        end
        subgraph graphService["TrackService"]
            TrackService
            Track
            Node
        end
    end

    AudioSession --> DeviceService
    AudioSession --> FileService
    AudioSession --> TrackService

    DeviceService --> Device
    FileService --> File
    TrackService --> Track
    Track --> Node
```

\newpage

## 3.3. Logical View

### 3.3.1. Record Audio/MIDI Input

| Design Concern |     |
| -------------- | --- |
| **DC-01** | Monitor audio input device.
| **DC-02** | Monitor MIDI input device.

```mermaid
flowchart LR

    Step1["Get Input Devices"]
    Step2["Select Input Device"]
    Step3["Record"]
    Step4["Stop"]

    Step1 --> Step2
    Step2 --> Step3
    Step3 -->|...| Step4
```

### 3.3.2. Play Audio/MIDI File

| Design Concern | |
| -- | -- |
| **DC-03** | Open and read WAV audio files.
| **DC-04** | Open and read MIDI files.

```mermaid
flowchart LR

    Step1["Read Filesystem"]
    Step2["Open File"]
    Step3["Play"]
    Step4["Stop"]

    Step1 --> Step2
    Step2 --> Step3
    Step3 -->|...| Step4
```

### 3.3.3. Playback to Output Device

| Design Concern |     |
| -------------- | --- |
| **DC-05** | Route audio to output device.
| **DC-06** | Route MIDI to output device.

```mermaid
flowchart LR

    Step1["Get Output Devices"]
    Step2["Select Output Device"]
    Step3["Play"]
    Step4["Stop"]

    Step1 --> Step2
    Step2 --> Step3
    Step3 -->|...| Step4
```

### 3.3.4. Read MIDI Messages

| Design Concern | |
| -- | -- |
| **DC-07** | Processing incoming MIDI messages.

```mermaid
flowchart LR

    Step1["Get Input Devices"]
    Step2["Select Input Device"]
    Step4["Set Input Message Handler"]
    Step5["Play"]
    Step6["Stop"]

    Step1 --> Step2
    Step2 --> Step4
    Step4 --> Step5
    Step5 --> |...|Step6
```

\newpage

### 3.3.5. Audio Processing

| Design Concern | |
| -- | -- |
| **DC-08** | Processing incoming audio streams.

```mermaid
flowchart LR

    Step1["Get Input Devices"]
    Step2["Select Audio Input Device"]
    Step3["Select Audio Output Device"]
    Step4["Add Audio Processor"]
    Step5["Play"]
    Step6["Stop"]

    Step1 --> Step2
    Step2 --> Step3
    Step3 --> Step4
    Step4 --> Step5
    Step5 --> |...|Step6
```

\newpage

### 3.3.6. Multiple Tracks

| Design Concern |                                               |
| -------------- | --------------------------------------------- |
| **DC-09**      | Manage multiple audio tracks.                 |
| **DC-10**      | Add one audio or MIDI input to a track.       |
| **DC-11**      | Attach one audio or MIDI output to a track.   |
| **DC-12**      | Chain multiple audio processors in one track. |

```mermaid
flowchart LR

    subgraph Node["Track"]
        Step2["Select Input Device / File"]
    end

    subgraph Track2["Track"]
        Step22["Select Input Device / File"]
    end

    SelectOutput["Select Output Device"]

    J((" "))

    Step4["Play"]
    Step5["Stop"]

    SelectOutput --> Step2
    SelectOutput --> Step22 

    Step4 -->|...| Step5

    Step2 --> J
    Step22 --> J

    J --> Step4
```

\newpage

## 3.4. Dependency View

The components in this SDK depend on the file system, audio and MIDI devices on the host system.
Separating devices and files into different services divides the dependency.

### 3.4.1. Layer Hierarchy

```mermaid
graph TD

    subgraph public
        AudioSession
    end

    subgraph services
        graphService
        deviceService
        fileService
    end

    subgraph graphService["TrackService"]
        TrackService
        Track
        Node
    end

    subgraph deviceService["Device Service"]
        DeviceService
        Device
    end

    subgraph fileService["File Service"]
        FileService
        File
    end

    subgraph adapters
        FileAdapter
        AudioAdapter
        MidiAdapter
    end

    AudioSession --> graphService
    AudioSession --> deviceService
    AudioSession --> fileService

    AudioSession --> AudioAdapter
    AudioSession --> MidiAdapter
    AudioSession --> FileAdapter

    TrackService --> Track
    DeviceService --> Device
    FileService --> File

    Track --> Node
```

\newpage

## 3.5. Information View

### 3.5.1. Audio Session

### 3.5.2. Device Service

```mermaid
classDiagram

    class DeviceService {
        -devices : list<Device>
    }

    class Device {
        +get_id() unsigned int
        +get_name() string
        +get_device_type() eDeviceType
        +is_input() bool
        +is_output() bool
        +is_default_input() bool
        +is_default_output() bool
        +get_output_channels() unsigned int
        +get_input_channels() unsigned int
        +get_sample_rates() vector~unsigned int~
        +get_preferred_sample_rate() unsigned int
        +to_string() string
    }

    class AudioDevice {

    }

    class MidiDevice {

    }

    DeviceService *-- Device
    Device <|-- AudioDevice
    Device <|-- MidiDevice
```

### 3.5.3. File Service

```mermaid
classDiagram

    class FileService {
        -files : list<File>
    }

    class File {
        +get_file_type() eFileType
        +get_filepath() path
        +get_filename() string
        +get_total_frames() unsigned int
        +get_sample_rate() unsigned int
        +get_channels() unsigned int
        +get_duration_seconds() double
        +get_format_string() string
        +read_frames(vector~float~, long long) long long
        +seek(long long)
        +to_string() string
    }

    class AudioFile {

    }

    class MidiFile {

    }

    FileService *-- File
    File <|-- AudioFile
    File <|-- MidiFile
```

### 3.5.4. Track Service

```mermaid
classDiagram

    class Track {
        +get_nodes()
        +add_input_node()
        +add_processing_node()
        +add_output_node()
        +delete_node()
        +clear_nodes()

        +play()
        +record()
        +stop()
    }

    class Node {
        +play() bool
        +stop() bool
        +is_playing() bool
        +has_parent() bool
        +get_parent() Node
        +get_children() list~Node~
        #m_parent weak_ptr~Node~
        #m_children list~Node~
    }

    class InputNode {
        -port : Port~Device, File~
        -output : Node
    }

    class OutputNode {
        -port : Port~Device, File~
        -input : Node
    }

    class ProcessorNode {
        -input : Node
        -output : Node
    }

    class MixerNode {
        -inputs : list~Node~
        -output : Node
    }

    Track *-- Node

    Node <|-- InputNode
    Node <|-- ProcessorNode
    Node <|-- MixerNode
    Node <|-- OutputNode
```

\newpage

## 3.6. Interface View

### 3.6.1. Public SDK

The user uses the software via the main `miniaudioengine` SDK library.

The following types need to be accessible to the user:\newline
- `Device`\newline
- `File`\newline
- `Node`\newline
- `Processor`\newline

The following operations need to be accessible to the user:\newline
- Get audio/MIDI devices.\newline
- Get audio/MIDI files.\newline
- Set audio/MIDI device as input or output.\newline
- Set audio/MIDI file as input or output.\newline
- Add a new track.
- Add and audio or MIDI processor to a track.\newline
- Start/stop playback.\newline
- Start/stop recording.\newline
- Start/stop monitoring.\newline

\newpage

```mermaid
classDiagram

    class AudioSession {
        +get_audio_devices() list~AudioDevice~
        +get_midi_devices() list~MidiDevice~
        +add_track()
        +play()
        +stop()
        +record()
        +mute()
    }

    class Track {
        +add_input_node(InputNode)
        +add_output_node(OutputNode)
        +add_processor_node(ProcessorNode)
        +play()
        +stop()
        +record()
        +mute()
    }

    Track --> InputNode
    Track --> OutputNode
    Track --> ProcessorNode

    AudioSession --> Track
    AudioSession --> Device
    AudioSession --> File
```

## 3.7. Interaction View

### 3.7.1 Play Audio File

```mermaid
sequenceDiagram

    participant App
    participant AudioSession
    participant FileService
    participant DeviceService
    participant TrackService
    participant Track
    participant AudioAdapter
    participant FileAdapter

    App->>AudioSession: get_audio_devices()
    AudioSession->>DeviceService: get_audio_devices()
    DeviceService->>AudioAdapter: get_audio_devices()

    App->>AudioSession: get_audio_files()
    AudioSession->>FileService: get_audio_files()
    FileService->>FileAdapter: get_audio_files()

    App->>AudioSession: add_track()
    AudioSession->>TrackService: add_track()

    App->>Track: add_output_device()
    App->>Track: add_input_device()

    App->>AudioSession: play()
    AudioSession->>TrackService: play()
    TrackService->>Track: play()
    Track->>AudioAdapter: play()

    App->>AudioSession: stop()
    AudioSession->>TrackService: stop()
    TrackService->>Track: stop()
    Track->>AudioAdapter: stop()
```

\newpage

## 3.8. Structure View

### 3.8.1. Library Structure

| Library | Description |
|---|---|
| framework |
| services |
| adapters |
| RtAudio |
| RtMidi |
| sndfile |

```mermaid
graph TD
    subgraph miniaudioengine

        AudioSession

        subgraph framework
            IService
            IAdapter
            IHandle
            INode
        end

        subgraph services
            DeviceService
            FileService
            TrackService
            Track
            Node
            File
            Device
        end

        subgraph adapters
            FileAdapter
            AudioAdapter
            MidiAdapter
        end

    end

    subgraph External
        RtAudio
        RtMidi
        sndfile
    end

    AudioSession --> services

    TrackService --> Track
    Track --> Node
    DeviceService --> Device
    FileService --> File

    services --> adapters
    adapters --> External
    services --> framework
    adapters --> framework
```

### 3.8.2. Project Structure

```bash
examples/           # Example programs using miniaudioengine SDK
include/
    miniaudioengine/
        audiosession.h          # using namespace miniaudioengine
        track/
            track.h             # using namespace miniaudioengine
            input.h             # using namespace miniaudioengine
            output.h            # using namespace miniaudioengine
            processor.h         # using namespace miniaudioengine
        device/
            audiodevice.h       # using namespace miniaudioengine
            mididevice.h        # using namespace miniaudioengine
        file/
            audiofile.h         # using namespace miniaudioengine
            midifile.h          # using namespace miniaudioengine
src/
    framework/
        include/
          interfaces/
              service.h         # using namespace miniaudioengine::interfaces
              adapter.h         # using namespace miniaudioengine::interfaces
              handle.h          # using namespace miniaudioengine::interfaces
              node.h            # using namespace miniaudioengine::interfaces
        src/
            audiosession.cpp
    services/
        track/
            include/
                trackservice.h          # using namespace miniaudioengine::services
                mixer.h
            src/
                trackservice.cpp        # using namespace miniaudioengine::services
                track.cpp
                input.cpp
                output.cpp
                processor.cpp
                mixer.cpp
        device/
            include/
                deviceservice.h         # using namespace miniaudioengine::services
            src/
                deviceservice.cpp       # using namespace miniaudioengine::services
                audiofile.cpp
                midifile.cpp
        file/
            include/
                fileservice.h           # using namespace miniaudioengine::services
            src/
                fileservice.cpp         # using namespace miniaudioengine::services
                audiofile.cpp
                midifile.cpp
    adapters/
        include/
            audioadapter.h              # using namespace miniaudioengine::adapters
            midiadapter.h               # using namespace miniaudioengine::adapters
            fileadapter.h               # using namespace miniaudioengine::adapters
        src/
            audioadapter.cpp            # using namespace miniaudioengine::adapters
            midiadapter.cpp             # using namespace miniaudioengine::adapters
            fileadapter.cpp             # using namespace miniaudioengine::adapters
tests/
```

### 3.8.3. Thread Structure

| Thread | Description |
|---|---|
| Main | Main execution thread.
| RtAudio | Real-time audio data processing.
| RtMidi | Real-time MIDI message handling. 


```mermaid
graph TD
    subgraph Main
        App
    end

    subgraph RtAudio
        AudioCallbackHandler
    end

    subgraph RtMidi
        MidiCallbackHandler
    end

    Main <--> RtAudio
    Main <--> RtMidi
```

\newpage

# 4. Design Rationale

## 4.1. Architectural Design

I am keeping the control and data components of this library separate. The main component of the control section is the `AudioSession`. The `AudioSession` owns `Services` for different functionalities. `AudioSession` wraps function calls to the internal `Services` and exposes them to the programmer. `Services` own `Adapters` to interface with external hardware / libraries. `Services` return `Handle` objects for the programmer to interact with.

### 4.1.1. Layered Architecture

**Structural Roles**

| Concept | Control Plane |	Data Plane
| --- | --- | --- |
Entry point |	AudioSession	 | MainTrack (audio callback)
Per-track unit	| Node (configuration) |	AudioDataPlane / MidiDataPlane
Cross-thread comms	| Services (locks OK) |	LockfreeRingBuffer
Lifetime manager |	TrackService |	AudioController |

### 4.1.2. Node Hierarchy

```mermaid
graph TD

    TrackService --> MainTrack
    MainTrack --> Track1["Node"]
    MainTrack --> Track2["Node"]
    MainTrack --> Track3["Node"]
```

### 4.1.3. Software Design Patterns

**C++ PImpl**

**Facade**

This SDK uses the Facade pattern for the `AudioSession` object. `AudioSession` is a wrapper around the internal logic exposed to the programmer. An `AudioSession` owns a `DeviceService`, `FileService`, and `TrackService` that divides and implements the internal engine logic.

```mermaid
classDiagram

    class IFacade {
        +foo() int
        -object : IObject
    }

    class IObject {
        +foo() int
    }

    IFacade *-- IObject
```

where

```cpp
int IFacade::foo() {
    return object.foo();
}
```

*e.g.*

```mermaid
classDiagram

    class IFacade
    class IObject

    class AudioSession
    class DeviceService
    class FileService
    class TrackService

    IFacade <|-- AudioSession
    IObject <|-- DeviceService
    IObject <|-- FileService
    IObject <|-- TrackService

    DeviceService --* AudioSession
    FileService --* AudioSession
    TrackService --* AudioSession
```

**Adapter**

**Factory**

This SDK uses the Factory pattern to create `Device`, `File`, and `Node` objects.

```mermaid
classDiagram

    class IFactory {
        +createObject() IObject
    }

    class IObject {
        -IObject()
    }

    IFactory --> IObject
```

*e.g.*

```mermaid
classDiagram

    class IFactory
    class IObject

    class DeviceAdapter

    IFactory <|-- DeviceAdapter
    IObject <|-- Device

    DeviceAdapter --> Device
```

## 4.2. External Libraries
