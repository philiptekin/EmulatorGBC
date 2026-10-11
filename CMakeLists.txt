cmake_minimum_required(VERSION 3.24)

project(EmulatorGBC
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

option(ENABLE_CLANG_TIDY "Enable clang-tidy analysis" OFF)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
endif()

# Production code shared by the emulator and the tests
add_library(gameboy_core
    src/core/memory/Wram.cpp
    src/core/memory/Hram.cpp
    src/core/memory/Vram.cpp
    src/core/memory/Bus.cpp
    src/core/cpu/Cpu.cpp
)

if(ENABLE_CLANG_TIDY)
    set_property(TARGET gameboy_core
        PROPERTY CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
    )
endif()

target_include_directories(gameboy_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(
    EmulatorGBC
    src/main.cpp
    src/core/EmulatorDMG.cpp
)

if(ENABLE_CLANG_TIDY)
    set_property(TARGET EmulatorGBC
        PROPERTY CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
    )
endif()

target_link_libraries(EmulatorGBC
    PRIVATE
        gameboy_core
)

if (BUILD_TESTING)
    include(FetchContent)

    fetchcontent_declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.18.0
        GIT_SHALLOW TRUE
    )

    fetchcontent_makeavailable(googletest)
    add_subdirectory(tests)
endif()
