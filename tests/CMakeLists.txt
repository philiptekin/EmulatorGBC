add_executable(memory_tests
    memory/HramTests.cpp
    memory/WramTests.cpp
    memory/VramTests.cpp
)

add_executable(cpu_tests
    cpu/CpuTests.cpp
    cpu/RegistersTests.cpp
)

if(ENABLE_CLANG_TIDY)
    set_property(TARGET memory_tests
        PROPERTY CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
    )
    set_property(TARGET cpu_tests
        PROPERTY CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
    )
endif()

target_link_libraries(memory_tests
    PRIVATE
        gameboy_core
        gtest_main
)

target_link_libraries(cpu_tests
    PRIVATE
        gameboy_core
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(memory_tests)
gtest_discover_tests(cpu_tests)
