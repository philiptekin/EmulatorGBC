add_executable(memory_tests
    memory/HramTests.cpp
    memory/WramTests.cpp
    memory/VramTests.cpp
)

if(ENABLE_CLANG_TIDY)
    set_property(TARGET memory_tests
        PROPERTY CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
    )
endif()

target_link_libraries(memory_tests
    PRIVATE
        gameboy_core
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(memory_tests)
