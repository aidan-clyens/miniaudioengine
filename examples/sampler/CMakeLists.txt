cmake_minimum_required(VERSION 3.25)

project(sampler
    VERSION 1.0.0
    DESCRIPTION "Sampler Example Project"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

# Installation settings
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS sampler
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install runtime dependencies (DLLs) on Windows
if(WIN32)
    # Install all DLLs from vcpkg bin directory
    file(GLOB VCPKG_DLLS "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin/*.dll")
    install(
        FILES ${VCPKG_DLLS}
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Runtime
    )
    
    # Also try to install target runtime DLLs (for any non-vcpkg dependencies)
    install(
        FILES
            $<TARGET_RUNTIME_DLLS:sampler>
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Runtime
    )
endif()
