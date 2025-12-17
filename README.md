## 1. Introduction
minimal-audio-engine is a cross-platform, cross-architecture, modern C++ library designed to be the backend of a digital audio processing workstation / application. It is a library that can be included by and used to build digital audio processing applications. For examples, these may include an audio plugin VST, a digital audio workstation, or an embedded audio platform.

This specification defines the design, build environment, architecture, and operational requirements of the system. It is intended to document engineering decisions, ensure reproducibility, and provide a foundation for future extensions.

My intention with this project is to create a modern C++20 application using the most up-to-date best practices and standards. I want to leverage concurrency and focus on mastering thread-safe programming and parallel design paradigms.

---

## 2. Objectives
- Deliver a **minimal yet extensible audio engine** suitable for embedded and desktop environments.  
- Ensure **cross-platform support** across Linux and Windows.  
- Support **multi-architecture builds** (x86_64, ARM64).  
- Provide a **Dockerized development environment** for reproducibility.  
- Implement **continuous integration (CI/CD)** for automated builds, testing, and release packaging.  

---

## 3. System Overview
minimal-audio-engine consists of:  
- **Libraries**: Implements a digital audio processing framework that may be used to develop audio software.
  - framework
  - audioengine
  - midiengine
  - trackmanager
  - devicemanager
  - filemanager 
- **Examples:**: Examples of audio software applications using minimal-audio-engine.

---

## 4. Build Environment
### 4.1 Setup for Linux
#### 4.1.1 x86
#### 4.1.2 ARM64

### 4.2 Setup for Windows

### 4.3 CI/CD

---

## 5. Architecture
### 5.1 High-Level Design
```
+-------------------------+
|   Application Layer     |
|  (Audio Workstation UI) |
+-------------------------+
            |
            v
+-------------------------+
|   Core Audio Engine     |
|  (DSP, Processing)      |
+-------------------------+
            |
            v
+-------------------------+
| Platform Abstraction    |
| (Linux / Windows APIs)  |
+-------------------------+
            |
            v
+-------------------------+
| Hardware Layer          |
| (x86_64 / ARM64)        |
+-------------------------+
```

### 5.2 Project Structure
```
minimal-audio-engine/
├── src/                # Source code
├──── framework/        # Framework library
├──── audioengine/      # Audio Engine library
├──── midiengine/       # MIDI Engine library
├──── trackmanager/     # Track Manager library
├──── devicemanager/    # Device Manager library
├──── filemanager/      # File Manager library
├── include/            # Public headers
├── tests/              # Unit tests
├── examples/           # Examples
├──── wav-audio-player/ # WAV Audio File Player example program
├── CMakeLists.txt      # Build configuration
├── CMakePresets.json   # Build configuration presets
├── Dockerfile          # Development environment
└── .github/workflows/  # CI/CD pipeline
```

---

## 6. Continuous Integration (CI/CD)
The repository uses **GitHub Actions** to automate:  
1. Multi-arch Docker image builds (linux/amd64, linux/arm64).
2. Library and example builds
3. Packaging of build artifacts (`.tar.gz`).

