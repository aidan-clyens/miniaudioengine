cmake_minimum_required(VERSION 3.25)

# Enable automatic DLL copying
cmake_policy(SET CMP0087 NEW)

project(midi-device-input
    VERSION 1.0.0
    DESCRIPTION "A simple MIDI device input example using Minimal Audio Engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

# Installation settings
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# install(TARGETS midi-device-input
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# # Install runtime dependencies (DLLs) on Windows
# if(WIN32)
#     # Install all DLLs from vcpkg bin directory
#     file(GLOB VCPKG_DLLS "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin/*.dll")
#     install(
#         FILES ${VCPKG_DLLS}
#         DESTINATION ${CMAKE_INSTALL_BINDIR}
#         COMPONENT Runtime
#     )
    
#     # Also try to install target runtime DLLs (for any non-vcpkg dependencies)
#     install(
#         FILES
#             $<TARGET_RUNTIME_DLLS:midi-device-input>
#         DESTINATION ${CMAKE_INSTALL_BINDIR}
#         COMPONENT Runtime
#     )
# endif()
