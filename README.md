# Minimal Audio Engine

The Minimal Audio Engine is a C++ project designed for cross-platform minimal audio processing on lightweight computing platforms like the Raspberry Pi.

# Build Environment

- Cross-platform build support (x86_64 and ARM64)
- Cross-OS build support (Linux and Windows)
- Docker-based development environment
- CMake build system

## Prerequisites

Make sure you have the following installed on your Ubuntu system:

- Docker
- Docker Buildx (usually included with recent Docker versions)
- Git

## Clone the Repository

git clone https://github.com/<your-username>/minimal-audio-engine.git
cd minimal-audio-engine

## Build with Docker

The project provides a multi-arch Docker image published to GitHub Container Registry (GHCR).This ensures a consistent build environment across AMD64 and ARM64.

### 1. Pull the Development Image

```bash
docker pull ghcr.io/aidan-clyens/minimal-audio-engine-dev:latest
```

### 2. Run the Container

Mount the repository into the container and start a shell:

```bash
docker run --rm -it \
  -v $(pwd):/workspace \
  -w /workspace \
  ghcr.io/aidan-clyens/minimal-audio-engine-dev:latest \
  bash
```

### 3. Build the Project with CMake

Inside the container:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build
```

The compiled binaries will be available in the build/ directory.

## Architecture-Specific Builds

You can explicitly build for AMD64 or ARM64 using Docker’s --platform flag.

### Build for AMD64

```bash
docker run --rm -it \
  --platform linux/amd64 \
  -v $(pwd):/workspace \
  -w /workspace \
  ghcr.io/aidan-clyens/minimal-audio-engine-dev:latest \
  bash -c "cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel && ctest --test-dir build"
```

### Build for ARM64

```bash
docker run --rm -it \
  --platform linux/arm64 \
  -v $(pwd):/workspace \
  -w /workspace \
  ghcr.io/aidan-clyens/minimal-audio-engine-dev:latest \
  bash -c "cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel && ctest --test-dir build"
```

# Continuous Integration (CI)

This repository uses GitHub Actions to:
1. Build and push a multi-arch Docker image (linux/amd64, linux/arm64)
2. Compile and test the project inside the container
3. Package build artifacts (.tar.gz) for both architectures
4. Publish nightly releases with prebuilt binaries

You can find the latest prebuilt binaries under the Releases page.

# Project Structure

```
minimal-audio-engine/
├── CMakeLists.txt      # CMake build configuration
├── Dockerfile          # Build environment definition
├── src/                # Source code
├── include/            # Public headers
├── tests/              # Unit tests
└── .github/workflows/  # CI/CD pipeline
```
