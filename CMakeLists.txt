cmake_minimum_required(VERSION 3.14)
project(c_minus_compiler C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_library(project_includes INTERFACE)
target_include_directories(project_includes INTERFACE
    ${PROJECT_SOURCE_DIR}
)

add_executable(data_structures_test
    src/data_structures/vector.c
    src/data_structures/hashmap.c
    src/data_structures/queue.c
    src/helpers/hash.c
    src/helpers/math.c
    tests/data_structures/vector_test.cpp
    tests/data_structures/hashmap_test.cpp
    tests/data_structures/queue_test.cpp
)
target_link_libraries(data_structures_test PRIVATE GTest::gtest_main project_includes)

gtest_discover_tests(data_structures_test)

add_library(hashmap OBJECT src/data_structures/hashmap.c)
target_link_libraries(hashmap PRIVATE project_includes)

add_executable(helpers_test
    src/helpers/hash.c
    src/helpers/math.c
    tests/helpers/hash_test.cpp
    tests/helpers/math_test.cpp
)
target_link_libraries(helpers_test PRIVATE GTest::gtest_main project_includes)

gtest_discover_tests(helpers_test)

add_library(automata OBJECT src/automata/automata.c)
target_link_libraries(automata PRIVATE project_includes)

add_library(stack OBJECT src/data_structures/stack.c)
target_link_libraries(stack PRIVATE project_includes)

add_library(queue OBJECT src/data_structures/queue.c)
target_link_libraries(queue PRIVATE project_includes)
