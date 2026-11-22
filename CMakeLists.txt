cmake_minimum_required(VERSION 3.23)
project(Scheduler LANGUAGES  C)

add_subdirectory(unit_tests)

target_include_directories(sche_tests PRIVATE
    inc acutest/include
)

target_compile_definitions(sche_tests PRIVATE
    ENABLE_UNIT_TEST
)

add_subdirectory(src)