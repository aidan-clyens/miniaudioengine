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
| **DC-01** | Monitor audio input device. | Logical, Interface, Interaction
| **DC-02** | Monitor MIDI input device. | Logical, Interface, Interaction
| **DC-03** | Open and read WAV audio files. | Logical, Information, Interaction
| **DC-04** | Open and read MIDI files. | Logical, Information
| **DC-05** | Route audio to output device. | Logical, Interface, Interaction
| **DC-06** | Route MIDI to output device. | Logical, Interface
| **DC-07** | Processing incoming MIDI messages. | Logical, Interface
| **DC-08** | Processing incoming audio streams. | Logical, Information, Interface
| **DC-09** | Manage multiple audio tracks. | Logical, Information, Interface, Interaction
| **DC-10** | Add one audio or MIDI input to a track. | Logical, Information, Interface
| **DC-11** | Attach one audio or MIDI output to a track. | Logical, Information, Interface
| **DC-12** | Chain multiple audio processors in one track. | Logical, Information, Interface
| **DC-13** | Build software SDK on Windows and Linux. | Context, Structure
| **DC-14** | Build software SDK on x86_64 and ARM64 platforms. | Context, Structure
| **DC-15** | CI/CD pipeline builds and packages software on all compatible platforms. | Structure
| **DC-16** | Complete unit testing and code coverage. | Structure
| **DC-17** | Package software as an SDK used by third-party software. | Context, Composition, Structure
| **DC-18** | Third-party software developers manage audio tracks, system audio/MIDI devices, and filesystem. | Composition, Interface, Interaction

\newpage

# 3. Design Views

## 3.1. Context View

Describes the software in context with its external environment. Define users, external components, and the interactions between.

| Design Concern | |
| -- | -- |
| **DC-13** | Build software SDK on Windows and Linux. |
| **DC-14** | Build software SDK on x86_64 and ARM64 platforms. |
| **DC-17** | Package software as an SDK used by third-party software. |

```mermaid caption="miniaudioengine SDK environment context" width=80%
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

### 3.8.1. SDK Components

Describe the composition of the **miniaudioengine** SDK software libraries.

| Design Concern | |
| -- | -- |
| **DC-17** | Package software as an SDK used by third-party software. |
| **DC-18** | Third-party software developers manage audio tracks, system audio/MIDI devices, and filesystem. |

```mermaid caption="miniaudioengine SDK composition" height=70%
graph TD
    subgraph SDK["miniaudioengine SDK"]
        AudioSession

        subgraph Services
            TrackService
            DeviceService
            FileService
        end

        subgraph Entities
            Track
            Device
            File
        end

        subgraph Adapters
            AudioAdapter
            MidiAdapter
            FileAdapter
        end
    end

    App --> AudioSession

    AudioSession --> Services
    AudioSession --> Adapters
    Services --> Entities
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

    getInputDevices("Get Input Devices")

    subgraph Track
        selectInputDevice("Set Input Device")
    end

    record("Record")
    stop("Stop")

    getInputDevices --> selectInputDevice
    selectInputDevice --> record
    record -->|...| stop
```

### 3.3.2. Play Audio/MIDI File

| Design Concern | |
| -- | -- |
| **DC-03** | Open and read WAV audio files.
| **DC-04** | Open and read MIDI files.

```mermaid
flowchart LR

    readFiles("Read Filesystem")

    subgraph Track
        openFile("Open File")
    end

    play("Play")
    stop("Stop")

    readFiles --> openFile
    openFile --> play
    play -->|...| stop
```

### 3.3.3. Playback to Output Device

| Design Concern |     |
| -------------- | --- |
| **DC-05** | Route audio to output device.
| **DC-06** | Route MIDI to output device.

```mermaid
flowchart LR

    getOutputDevices("Get Output Devices")

    subgraph Track
        selectOutputDevice("Set Output Device")
    end

    play("Play")
    stop("Stop")

    getOutputDevices --> selectOutputDevice
    selectOutputDevice --> play
    play -->|...| stop
```

### 3.3.4. Audio/MIDI Processing

| Design Concern | |
| -- | -- |
| **DC-07** | Processing incoming MIDI messages.
| **DC-08** | Processing incoming audio streams.

```mermaid
flowchart LR

    getInputDevices("Get Input Devices")

    subgraph Track
        selectInputDevice("Set Input Device")
        addProcessor("Add Processor")
        selectOutputDevice("Set Output Device")
    end

    play("Play")
    stop("Stop")

    getInputDevices --> selectInputDevice
    selectInputDevice --> addProcessor
    addProcessor --> selectOutputDevice
    selectOutputDevice --> play
    play --> |...|stop
```

\newpage

### 3.3.5. Multiple Tracks

| Design Concern |                                               |
| -------------- | --------------------------------------------- |
| **DC-09**      | Manage multiple audio tracks.                 |
| **DC-10**      | Add one audio or MIDI input to a track.       |
| **DC-11**      | Attach one audio or MIDI output to a track.   |
| **DC-12**      | Chain multiple audio processors in one track. |

```mermaid
flowchart LR

    getInputs("Get Inputs")
    getOutputs("Get Outputs")

    subgraph track1["Track"]
        selectInputTrack1("Set Input")
        addProcessorTrack1("Add Processor")
        selectOutputTrack1("Set Output")
    end

    subgraph track2["Track"]
        selectInputTrack2("Set Input")
        addProcessorTrack2("Add Processor")
        selectOutputTrack2("Set Output")
    end

    J((" "))

    play("Play")
    stop("Stop")

    getInputs --> getOutputs
    getOutputs --> selectInputTrack1
    getOutputs --> selectInputTrack2

    selectInputTrack1 --> addProcessorTrack1
    addProcessorTrack1 --> selectOutputTrack1

    selectInputTrack2 --> addProcessorTrack2
    addProcessorTrack2 --> selectOutputTrack2

    play -->|...| stop

    selectOutputTrack1 --> J
    selectOutputTrack2 --> J

    J --> play
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
        TrackService
        DeviceService
        FileService
    end

    subgraph entities
        Track
        Device
        File
    end

    subgraph adapters
        FileAdapter
        AudioAdapter
        MidiAdapter
    end

    public --> adapters
    public --> services
    public --> entities
    services --> entities
    services --> adapters
    adapters --> entities
```

\newpage

### 3.4.2. External Library Adapters

\newpage

## 3.5. Information View

| Design Concern | |
| -- | -- |
| **DC-03** | Open and read WAV audio files. |
| **DC-04** | Open and read MIDI files. |
| **DC-08** | Processing incoming audio streams. |
| **DC-09** | Manage multiple audio tracks. |
| **DC-10** | Add one audio or MIDI input to a track. |
| **DC-11** | Attach one audio or MIDI output to a track. |
| **DC-12** | Chain multiple audio processors in one track. |

### 3.5.1. Audio Session

```mermaid
classDiagram

    class AudioSession {
        -track_service : TrackService
        -device_service : DeviceService
        -file_service : FileService
        -audio_adapter: AudioAdapter
        -midi_adapter: MidiAdapter
        -file_adapter: FileAdapter
        +get_audio_devices() list~AudioDevice~
        +get_midi_devices() list~MidiDevice~
        +get_audio_files() list~AudioFile~
        +get_midi_files() list~MidiFile~
        +play()
        +record()
        +stop()
    }

    AudioSession *-- TrackService
    AudioSession *-- DeviceService
    AudioSession *-- FileService
    AudioSession *-- AudioAdapter
    AudioSession *-- FileAdapter
    AudioSession *-- MidiAdapter
```

### 3.5.2. Device Service

```mermaid
classDiagram

    class DeviceService {
        -devices : list<Device>
        +DeviceService(audio_adapter : AudioAdapter, midi_adapter: MidiAdapter)
    }

    class Device {
        +get_id() unsigned int
        +get_name() string
        +get_device_type() eDeviceType
        +is_input() bool
        +is_output() bool
        +is_default_input() bool
        +is_default_output() bool
        +to_string() string
    }

    class AudioDevice {
        +get_output_channels() unsigned int
        +get_input_channels() unsigned int
        +get_sample_rates() vector~unsigned int~
        +get_preferred_sample_rate() unsigned int
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
        +FileService(adapter : FileAdapter)
    }

    class File {
        +get_file_type() eFileType
        +get_filepath() path
        +get_filename() string
        +to_string() string
    }

    class AudioFile {
        +get_total_frames() unsigned int
        +get_sample_rate() unsigned int
        +get_channels() unsigned int
        +get_duration_seconds() double
        +get_format_string() string
        +read_frames(vector~float~, long long) long long
        +seek(long long)
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

    class TrackService {
        -tracks : list~Track~
        +TrackService(audio_adapter : AudioAdapter, midi_adapter: MidiAdapter)
    }

    class Track {
        -input : InputNode
        -processors : list~ProcessorNode~
        -output : OutputNode

        +add_input() InputNode
        +add_processor() ProcessorNode
        +add_output() OutputNode

        +get_input() optional~InputNode~
        +get_processors() list~ProcessorNode~
        +get_output() optional~OutputNode~

        +remove_input()
        +remove_processor()
        +remove_output()

        +clear_processors()

        +play()
        +record()
        +stop()
    }

    class INode {


        +play()
        +record()
        +stop()
        
        +is_playing() bool
        
        +has_parent() bool
        +get_parent() optional~INode~
        +get_children() list~INode~
    }

    class InputNode {
        -port : variant~Device, File~
        -children : list~INode~
    }

    class OutputNode {
        -port : variant~Device, File~
        -parent : shared_ptr~INode~
    }

    class ProcessorNode {
        -p_input : shared_ptr~Node~
        -p_output : shared_ptr~Node~
    }

    class MixerNode {
        -p_inputs : list~shared_ptr~Node~~
        -p_output : shared_ptr~Node~
    }

    TrackService *-- Track

    Track *-- Node

    Node <|-- InputNode
    Node <|-- ProcessorNode
    Node <|-- MixerNode
    Node <|-- OutputNode
```

\newpage

## 3.6. Interface View

### 3.6.1. Public SDK

| Design Concern | |
| -- | -- |
| **DC-01** | Monitor audio input device. |
| **DC-02** | Monitor MIDI input device. |
| **DC-05** | Route audio to output device. |
| **DC-06** | Route MIDI to output device. |
| **DC-07** | Processing incoming MIDI messages. |
| **DC-08** | Processing incoming audio streams. |
| **DC-09** | Manage multiple audio tracks. |
| **DC-10** | Add one audio or MIDI input to a track. |
| **DC-11** | Attach one audio or MIDI output to a track. |
| **DC-12** | Chain multiple audio processors in one track. |
| **DC-18** | Third-party software developers manage audio tracks, system audio/MIDI devices, and filesystem. |

The user uses the software via the main `miniaudioengine` SDK library.

The following types need to be accessible to the user:\newline
- `AudioSession`\newline
- `Device`\newline
- `File`\newline
- `Track`\newline
- `InputNode`\newline
- `OutputNode`\newline
- `ProcessorNode`\newline

The following operations need to be accessible to the user:

| Operation | Component |
| --- | --- |
| Get audio/MIDI devices | `AudioSession`, `Device` |
| Get audio/MIDI files | `AudioSession`, `File` |
| Set audio/MIDI device as input or output | `Track`, `InputNode`, `OutputNode`|
| Set audio/MIDI file as input or output | `Track`, `InputNode`, `OutputNode`|
| Add a new track | `AudioSession`, `Track` |
| Add and audio or MIDI processor to a track | `Track`, `ProcessorNode` |
| Start/stop playback | `AudioSession`, `Track` |
| Start/stop recording | `AudioSession`, `Track` |
| Start/stop monitoring | `AudioSession`, `Track` |

```mermaid
classDiagram

    class AudioSession {
        +get_audio_devices() list~AudioDevice~
        +get_midi_devices() list~MidiDevice~
        +get_audio_files() list~AudioFile~
        +get_midi_files() list~MidiFile~
        +add_track() Track
        +play()
        +record()
        +stop()
        +monitor()
    }

    class Track {
        +add_input(InputNode)
        +add_output(OutputNode)
        +add_processor(ProcessorNode)
        +remove_node(Node)
        +play()
        +record()
        +stop()
        +monitor()
    }

    class InputNode {
        +InputNode(device : Device)
        +InputNode(file : File)
        +set_midi_handler(handler)
    }

    class OutputNode {
        +OutputNode(device : Device)
    }

    class ProcessorNode {
        +ProcessorNode(processor : Processor)
    }

    class Device {
        +get_id() unsigned int
        +get_name() string
        +is_input() bool
        +is_output() bool
    }

    class AudioDevice {
        +get_input_channels() unsigned int
        +get_output_channels() unsigned int
        +get_sample_rates() vector~unsigned int~
        +get_preferred_sample_rate() unsigned int
    }

    class MidiDevice {
    }

    class File {
        +get_file_type() eFileType
        +get_filepath() path
        +get_filename() string
        +to_string() string
    }

    class AudioFile {
        +get_total_frames() unsigned int
        +get_sample_rate() unsigned int
        +get_channels() unsigned int
        +get_duration_seconds() double
        +read_frames(vector~float~, long long) long long
        +seek(long long)
    }

    class MidiFile {
        +get_tempo() unsigned int
        +get_ticks_per_beat() unsigned int
        +get_track_count() unsigned int
    }

    AudioSession --> Track
    AudioSession --> Device
    AudioSession --> File
    Track --> InputNode
    Track --> OutputNode
    Track --> ProcessorNode
    Device <|-- AudioDevice
    Device <|-- MidiDevice
    File <|-- AudioFile
    File <|-- MidiFile
```

\newpage

## 3.7. Interaction View

| Design Concern | |
| -- | -- |
| **DC-03** | Open and read WAV audio files. |
| **DC-05** | Route audio to output device. |
| **DC-09** | Manage multiple audio tracks. |
| **DC-18** | Third-party software developers manage audio tracks, system audio/MIDI devices, and filesystem. |

### 3.7.1. Control Plane

- Get audio/MIDI devices\newline
- Get audio/MIDI files\newline
- Set audio/MIDI device as input or output\newline
- Set audio/MIDI file as input or output\newline
- Add a new track\newline
- Add and audio or MIDI processor to a track\newline
- Start/stop playback\newline
- Start/stop recording\newline

\newpage

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

### 3.7.2. Data Plane

```mermaid
flowchart LR
    inputA("Input A") --> input1
    inputB("Input B") --> input2

    subgraph track1["Track"]
        input1("Track Input") --> processor1("Track Processor")
        processor1 --> output1("Track Output")
    end

    subgraph track2["Track"]
        input2("Track Input") --> output2("Track Output")
    end

    output1 --> mixer("Mixer")
    output2 --> mixer("Mixer")

    mixer --> output("Output")
```

\newpage

```mermaid
graph TD

    TrackService --> AudioCallbackHandler
    AudioCallbackHandler --> Mixer
    Mixer --> OutputNode1("Output")
    Mixer --> OutputNode2("Output")
    Mixer --> OutputNode3("Output")

    subgraph Track1["Track"]
        OutputNode1 --> ProcessorNode1("Processor")
        ProcessorNode1 --> InputNode1("Input")
    end

    subgraph Track2["Track"]
        OutputNode2 --> ProcessorNode2("Processor")
        ProcessorNode2 --> InputNode2("Input")
    end

    subgraph Track3["Track"]
        OutputNode3 --> ProcessorNode3("Processor")
        ProcessorNode3 --> InputNode3("Input")
    end
```

\newpage

## 3.8. Structure View

| Design Concern | |
| -- | -- |
| **DC-13** | Build software SDK on Windows and Linux. |
| **DC-14** | Build software SDK on x86_64 and ARM64 platforms. |
| **DC-15** | CI/CD pipeline builds and packages software on all compatible platforms. |
| **DC-16** | Complete unit testing and code coverage. |
| **DC-17** | Package software as an SDK used by third-party software. |

### 3.8.1. Library Structure

| Internal Library | Components |
| --- | --- |
| miniaudioengine |
| services |
| adapters |
| entities |
| framework |

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
        end

        subgraph adapters
            FileAdapter
            AudioAdapter
            MidiAdapter
        end

        subgraph entities
            Track
            Node
            File
            Device
        end

    end

    subgraph External
        RtAudio
        RtMidi
        sndfile
    end

    AudioSession --> services
    AudioSession --> entities
    AudioSession --> adapters

    Track --> Node

    services --> entities
    adapters --> External
    services --> framework
    adapters --> framework
    entities --> framework
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
            mixer.h             # using namespace miniaudioengine
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
              device.h          # using namespace miniaudioengine::interfaces
              file.h            # using namespace miniaudioengine::interfaces
              node.h            # using namespace miniaudioengine::interfaces
        src/
            audiosession.cpp
    services/
        track/
            include/
                trackservice.h          # using namespace miniaudioengine::services
            src/
                trackservice.cpp        # using namespace miniaudioengine::services
        device/
            include/
                deviceservice.h         # using namespace miniaudioengine::services
            src/
                deviceservice.cpp       # using namespace miniaudioengine::services
        file/
            include/
                fileservice.h           # using namespace miniaudioengine::services
            src/
                fileservice.cpp         # using namespace miniaudioengine::services
    entities/
        include/
        src/
            audiodevice.cpp
            mididevice.cpp
            audiofile.cpp
            midifile.cpp
            track.cpp
            input.cpp
            output.cpp
            processor.cpp
            mixer.cpp
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

### 4.1.1. Audio Session

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

    App *-- AudioSession

    AudioSession *-- DeviceService
    AudioSession *-- FileService
    AudioSession *-- TrackService
```

### 4.1.2. Services

### 4.1.3. Adapters

This library uses the Adapter pattern to interface with external hardware / libraries. This adds an abstraction layer between the library and the external dependendies, providing isolation of implementation.

### 4.1.4. Handles

### 4.1.3. Software Design Patterns

**C++ PImpl**

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
