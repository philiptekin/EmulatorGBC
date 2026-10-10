cmake_minimum_required(VERSION 3.24)

project(EmulatorGBC
    VERSION 0.1.0
    LANGUAGES CXX
)

add_executable(
    EmulatorGBC
    src/main.cpp
)

target_compile_features(EmulatorGBC
    PRIVATE
        cxx_std_20
)   