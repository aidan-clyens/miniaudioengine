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
    # Enable automatic DLL copying
    cmake_policy(SET CMP0087 NEW)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux")
    add_compile_definitions(PLATFORM_LINUX)
endif()

# Find required packages
find_package(PkgConfig)

# RtAudio - Audio I/O library
if(WIN32)
    find_package(RtAudio CONFIG REQUIRED)
    add_library(rtaudio ALIAS RtAudio::rtaudio)
else()
    if(PkgConfig_FOUND)
        pkg_check_modules(RTAUDIO REQUIRED rtaudio)
        add_library(rtaudio INTERFACE)
        target_include_directories(rtaudio INTERFACE ${RTAUDIO_INCLUDE_DIRS})
        target_link_libraries(rtaudio INTERFACE ${RTAUDIO_LIBRARIES})
        target_link_directories(rtaudio INTERFACE ${RTAUDIO_LIBRARY_DIRS})
    endif()
endif()

# RtMidi - MIDI I/O library
if(WIN32)
    find_package(RtMidi CONFIG REQUIRED)
    add_library(rtmidi ALIAS RtMidi::rtmidi)
else()
    if(PkgConfig_FOUND)
        pkg_check_modules(RTMIDI REQUIRED rtmidi)
        add_library(rtmidi INTERFACE)
        target_include_directories(rtmidi INTERFACE ${RTMIDI_INCLUDE_DIRS})
        target_link_libraries(rtmidi INTERFACE ${RTMIDI_LIBRARIES})
        target_link_directories(rtmidi INTERFACE ${RTMIDI_LIBRARY_DIRS})
    endif()
endif()

# libsndfile - Audio file I/O library
if(WIN32)
    find_package(SndFile CONFIG REQUIRED)
    add_library(sndfile ALIAS SndFile::sndfile)
else()
    if(PkgConfig_FOUND)
        pkg_check_modules(SNDFILE REQUIRED sndfile)
        add_library(sndfile INTERFACE)
        target_include_directories(sndfile INTERFACE ${SNDFILE_INCLUDE_DIRS})
        target_link_libraries(sndfile INTERFACE ${SNDFILE_LIBRARIES})
        target_link_directories(sndfile INTERFACE ${SNDFILE_LIBRARY_DIRS})
    endif()
endif()

# Google Test - Unit testing framework
if(WIN32)
    find_package(GTest CONFIG REQUIRED)
else()
    find_package(GTest REQUIRED)
endif()

add_subdirectory(src)

option(BUILD_EXAMPLES "Build examples" ON)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

option(BUILD_TESTS "Build unit tests" ON)
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Installation settings
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install library targets and header files
install(
    TARGETS
        control-audio
        control-midi
        data-audio
        data-midi
        processor-audio
        trackmanager
        filemanager
        devicemanager
        framework
        cli
    EXPORT miniaudioengine-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/miniaudioengine
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