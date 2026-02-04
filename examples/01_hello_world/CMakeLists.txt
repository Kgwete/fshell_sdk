cmake_minimum_required(VERSION 3.10)

# ============================================================================
# Compiler Selection - FORCE GCC to match FShell SDK
# ============================================================================

# Remove the simple set() commands and use this robust approach instead
if(NOT DEFINED CMAKE_C_COMPILER)
    # Force GCC to match the FShell SDK build
    find_program(GCC_C NAMES gcc gcc.exe)
    if(GCC_C)
        set(CMAKE_C_COMPILER "${GCC_C}" CACHE FILEPATH "C compiler" FORCE)
    else()
        message(FATAL_ERROR "GCC not found in PATH. FShell SDK requires GCC.")
    endif()
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    # Force GCC to match the FShell SDK build
    find_program(GCC_CXX NAMES g++ g++.exe)
    if(GCC_CXX)
        set(CMAKE_CXX_COMPILER "${GCC_CXX}" CACHE FILEPATH "C++ compiler" FORCE)
    else()
        message(FATAL_ERROR "G++ not found in PATH. FShell SDK requires GCC.")
    endif()
endif()

# ============================================================================
# Generator Validation (Anti-Vendor Lock-in)
# ============================================================================

if(WIN32 AND NOT CMAKE_GENERATOR MATCHES "MinGW" AND NOT CMAKE_GENERATOR MATCHES "MSYS" AND NOT CMAKE_GENERATOR MATCHES "Unix")
    message(FATAL_ERROR
        "\n"
        "========================================================================\n"
        "ERROR: Invalid generator detected - Visual Studio/NMake not supported\n"
        "========================================================================\n"
        "\n"
        "FShell Hello World requires MinGW Makefiles generator with GCC.\n"
        "\n"
        "Please reconfigure with:\n"
        "\n"
        "  Option 1 - GCC (Required):\n"
        "    cmake -G \"MinGW Makefiles\" -S . -B build\n"
        "\n"
        "  Option 2 - MSYS2:\n"
        "    cmake -G \"MSYS Makefiles\" -S . -B build\n"
        "\n"
        "Make sure MinGW-w64 GCC is installed and in your PATH.\n"
        "========================================================================\n"
    )
endif()

project(FShellHelloWorld CXX)

# ============================================================================
# Post-Project Compiler Verification
# ============================================================================

# Verify we're using GCC (should be forced above)
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR
        "\n"
        "========================================================================\n"
        "ERROR: Wrong compiler detected!\n"
        "========================================================================\n"
        "\n"
        "FShell SDK was built with GCC. You must use GCC for compatibility.\n"
        "\n"
        "Detected: ${CMAKE_CXX_COMPILER_ID}\n"
        "Required: GNU (GCC)\n"
        "\n"
        "Solution:\n"
        "  cmake -G \"MinGW Makefiles\" -S . -B build\n"
        "\n"
        "========================================================================\n"
    )
endif()

# Double-check that MSVC didn't sneak in
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR MSVC)
    message(FATAL_ERROR
        "\n"
        "========================================================================\n"
        "ERROR: MSVC compiler detected despite safeguards!\n"
        "========================================================================\n"
        "\n"
        "This means you're using Visual Studio generator.\n"
        "\n"
        "Solution:\n"
        "  cmake -G \"MinGW Makefiles\" -S . -B build\n"
        "\n"
        "========================================================================\n"
    )
endif()

message(STATUS "✓ C++ Compiler: GCC ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "✓ C Compiler: GCC ${CMAKE_C_COMPILER_VERSION}")

# ============================================================================
# Build Configuration
# ============================================================================

# Default to Release if not specified (must match how FShell SDK was built)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    message(STATUS "Build type not specified, defaulting to Release")
endif()

# Set C++ standard - MUST MATCH FShell SDK's standard (C++23 from your SDK CMakeLists.txt)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")

# ============================================================================
# Project Configuration
# ============================================================================

# Path to FShell SDK (adjust if needed)
set(FSHELL_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# Include FShell headers
include_directories(${FSHELL_SDK_DIR}/include)

# Create the executable
add_executable(hello_world main.cc)

# ============================================================================
# Compiler Options - Must match FShell SDK options
# ============================================================================

# Use the same compiler options as FShell SDK
target_compile_options(hello_world PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
)

# Optimization flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(hello_world PRIVATE -O3 -DNDEBUG)
    message(STATUS "Build Type: Release (-O3)")
else()
    target_compile_options(hello_world PRIVATE -g -O0)
    message(STATUS "Build Type: Debug (-g -O0)")
endif()

# Platform-specific flags
if(WIN32)
    target_compile_definitions(hello_world PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    # Note: No -fPIC on Windows for executables
else()
    # Linux/macOS needs -fPIC for position-independent code
    target_compile_options(hello_world PRIVATE -fPIC)
endif()

# ============================================================================
# Library Linking
# ============================================================================

# Link against FShell library - MUST MATCH BUILD TYPE
if(WIN32)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FSHELL_LIB "${FSHELL_SDK_DIR}/lib/win/libfnshell_d.a")
    message(STATUS "Linking against Debug build: ${FSHELL_LIB}")
else()
    set(FSHELL_LIB "${FSHELL_SDK_DIR}/lib/win/libfnshell_r.a")
    message(STATUS "Linking against Release build: ${FSHELL_LIB}")
endif()

elseif(UNIX AND NOT APPLE)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FSHELL_LIB "${FSHELL_SDK_DIR}/lib/linux/libfnshell_d.a")
    message(STATUS "Linking against Debug build: ${FSHELL_LIB}")
else()
    set(FSHELL_LIB "${FSHELL_SDK_DIR}/lib/linux/libfnshell_r.a")
    message(STATUS "Linking against Release build: ${FSHELL_LIB}")
endif()

endif()
# Check if library exists
if(NOT EXISTS ${FSHELL_LIB})
    message(FATAL_ERROR 
        "FShell library not found: ${FSHELL_LIB}\n"
        "\n"
        "Please build the FShell SDK first with:\n"
        "  cd ${FSHELL_SDK_DIR}\n"
        "  rm -rf build\n"
        "  cmake -G \"MinGW Makefiles\" -B build\n"
        "  cmake --build build --config ${CMAKE_BUILD_TYPE}\n"
        "\n"
        "Make sure to use the same build type (Debug/Release) as this project.\n"
    )
endif()

# Static linking to avoid C++ runtime conflicts
if(WIN32)
    # On Windows with MinGW, use static linking to avoid runtime conflicts
    target_link_options(hello_world PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()

target_link_libraries(hello_world ${FSHELL_LIB})

# Platform-specific libraries
if(WIN32)
    target_link_libraries(hello_world ws2_32)
    message(STATUS "Platform: Windows (linking ws2_32)")
elseif(UNIX AND NOT APPLE)
    target_link_libraries(hello_world pthread dl)
    message(STATUS "Platform: Linux (linking pthread dl)")
elseif(APPLE)
    target_link_libraries(hello_world dl)
    message(STATUS "Platform: macOS (linking dl)")
endif()

# ============================================================================
# Build Summary
# ============================================================================

message(STATUS "")
message(STATUS "═══════════════════════════════════════════════════════════")
message(STATUS "FShell Hello World Configuration Summary")
message(STATUS "═══════════════════════════════════════════════════════════")
message(STATUS "  C++ Compiler:  GCC ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Build Type:    ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard:  C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Platform:      ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Architecture:  ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  FShell SDK:    ${FSHELL_SDK_DIR}")
message(STATUS "  Library:       ${FSHELL_LIB}")
message(STATUS "═══════════════════════════════════════════════════════════")
message(STATUS "")
message(STATUS "FShell Hello World example configured successfully!")

# ============================================================================
# Post-build verification
# ============================================================================

# Add a custom command to verify the build
add_custom_command(TARGET hello_world POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Build completed successfully!"
    COMMAND ${CMAKE_COMMAND} -E echo "Output: $<TARGET_FILE:hello_world>"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    VERBATIM
)