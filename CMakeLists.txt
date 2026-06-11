cmake_minimum_required(VERSION 3.25)

project(miniaudioengine
    VERSION 1.0.0
    LANGUAGES CXX
)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Platform detection
if(WIN32)
    message(STATUS "Building for Windows")
    add_compile_definitions(PLATFORM_WINDOWS)
    cmake_policy(SET CMP0087 NEW)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux")
    add_compile_definitions(PLATFORM_LINUX)
endif()

# Find required packages
find_package(PkgConfig REQUIRED)

option(USE_RTAUDIO "Use RtAudio for audio I/O" ON)
option(USE_RTMIDI "Use RtMidi for MIDI I/O" ON)

option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TESTS "Build unit tests" ON)

# RtAudio - Audio I/O library
if(USE_RTAUDIO)
    if(WIN32)
        find_package(RtAudio CONFIG REQUIRED)
        add_library(rtaudio ALIAS RtAudio::rtaudio)
    else()
        if(PkgConfig_FOUND)
            pkg_check_modules(RTAUDIO REQUIRED IMPORTED_TARGET rtaudio)
            add_library(rtaudio ALIAS PkgConfig::RTAUDIO)
        endif()
    endif()
endif()

# RtMidi - MIDI I/O library
if(USE_RTMIDI)
    if(WIN32)
        find_package(RtMidi CONFIG REQUIRED)
        add_library(rtmidi ALIAS RtMidi::rtmidi)
    else()
        if(PkgConfig_FOUND)
            pkg_check_modules(RTMIDI REQUIRED IMPORTED_TARGET rtmidi)
            add_library(rtmidi ALIAS PkgConfig::RTMIDI)
        endif()
    endif()
endif()

# libsndfile - Audio file I/O library
if(WIN32)
    find_package(SndFile CONFIG REQUIRED)
    add_library(sndfile ALIAS SndFile::sndfile)
else()
    if(PkgConfig_FOUND)
        pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)
        add_library(sndfile ALIAS PkgConfig::SNDFILE)
    endif()
endif()

# Google Test - Unit testing framework
if(BUILD_TESTS)
    if(WIN32)
        find_package(GTest CONFIG REQUIRED)
    else()
        find_package(GTest REQUIRED)
    endif()
endif()

add_subdirectory(src)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)  
endif()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Installation settings
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install library targets and header files
install(
    TARGETS
        audiosession
        entities
        adapters
        dataplane
        services
        framework
        cli
    EXPORT miniaudioengine-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets
install(EXPORT miniaudioengine-targets
    FILE miniaudioengine-targets.cmake
    NAMESPACE miniaudioengine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/miniaudioengine
)

# Create package configuration file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/miniaudioengine-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/miniaudioengine-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/miniaudioengine
)

# Create package version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/miniaudioengine-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install package configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/miniaudioengine-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/miniaudioengine-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/miniaudioengine
)

# CPack packaging configuration
set(CPACK_PACKAGE_NAME "lib${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Aidan Clyens")
set(CPACK_PACKAGE_CONTACT "Aidan Clyens <aidanclyens@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Minimal Audio Engine C++ SDK")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_RELOCATABLE ON)
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

# Debian/Ubuntu packaging configuration
if(UNIX AND NOT APPLE)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_NAME "lib${PROJECT_NAME}")
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_DEBIAN_PACKAGE_RELEASE "1")
    set(CPACK_DEBIAN_PACKAGE_LICENSE "MIT")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/aidan-clyens/miniaudioengine")
endif()

include(CPack)