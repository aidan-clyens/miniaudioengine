# Ubuntu/Linux toolchain for miniaudioengine
# Uses system packages installed via apt and pkg-config.

include_guard(GLOBAL)

if(NOT UNIX OR APPLE)
    message(FATAL_ERROR "This toolchain file is intended for Ubuntu/Linux builds only.")
endif()

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

# Detect the Ubuntu multiarch library directory.
if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)$")
    set(UBUNTU_MULTIARCH_TRIPLET "x86_64-linux-gnu")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(UBUNTU_MULTIARCH_TRIPLET "aarch64-linux-gnu")
else()
    set(UBUNTU_MULTIARCH_TRIPLET "")
endif()

# Prefer the system GCC toolchain when no compiler is explicitly provided.
if(NOT DEFINED CMAKE_C_COMPILER AND EXISTS "/usr/bin/gcc")
    set(CMAKE_C_COMPILER "/usr/bin/gcc" CACHE FILEPATH "Ubuntu C compiler")
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER AND EXISTS "/usr/bin/g++")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++" CACHE FILEPATH "Ubuntu C++ compiler")
endif()

# Project-wide Linux defaults.
set(CMAKE_C_STANDARD 17 CACHE STRING "C language standard")
set(CMAKE_C_STANDARD_REQUIRED ON CACHE BOOL "Require the configured C standard")
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ language standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Require the configured C++ standard")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Disable compiler-specific C++ extensions")
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "Build position-independent code by default")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile_commands.json")
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG OFF CACHE BOOL "Prefer module/pkg-config package discovery on Linux")

# Search roots for Ubuntu system libraries and headers.
list(APPEND CMAKE_PREFIX_PATH
    "/usr/local"
    "/usr"
)

list(APPEND CMAKE_INCLUDE_PATH
    "/usr/local/include"
    "/usr/include"
)

list(APPEND CMAKE_LIBRARY_PATH
    "/usr/local/lib"
    "/usr/lib"
)

if(NOT UBUNTU_MULTIARCH_TRIPLET STREQUAL "")
    list(APPEND CMAKE_LIBRARY_PATH "/usr/lib/${UBUNTU_MULTIARCH_TRIPLET}")
endif()

# Ensure pkg-config can locate apt-installed development packages.
set(_ubuntu_pkgconfig_paths "/usr/local/lib/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig")
if(NOT UBUNTU_MULTIARCH_TRIPLET STREQUAL "")
    string(PREPEND _ubuntu_pkgconfig_paths "/usr/lib/${UBUNTU_MULTIARCH_TRIPLET}/pkgconfig:")
endif()

if(DEFINED ENV{PKG_CONFIG_PATH} AND NOT "$ENV{PKG_CONFIG_PATH}" STREQUAL "")
    set(ENV{PKG_CONFIG_PATH} "${_ubuntu_pkgconfig_paths}:$ENV{PKG_CONFIG_PATH}")
else()
    set(ENV{PKG_CONFIG_PATH} "${_ubuntu_pkgconfig_paths}")
endif()

# Sensible defaults for single-config generators.
set(CMAKE_C_FLAGS_INIT "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_INIT "-Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG_INIT "-O0 -g3")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
