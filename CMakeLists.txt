cmake_minimum_required(VERSION 3.14)
project(c_minus_compiler C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(vector_test
    src/data_structures/vector.c
    tests/data_structures/vector_test.cpp
)
target_link_libraries(vector_test PRIVATE GTest::gtest_main)

target_include_directories(vector_test PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

gtest_discover_tests(vector_test)

add_executable(hash_test
    src/helpers/hash.c
    tests/helpers/hash_test.cpp
)
target_link_libraries(hash_test PRIVATE GTest::gtest_main)

target_include_directories(hash_test PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

gtest_discover_tests(hash_test)

add_library(hashmap OBJECT src/data_structures/hashmap.c)

target_include_directories(hashmap PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

add_executable(hashmap_test
    src/data_structures/hashmap.c
    src/data_structures/vector.c
    src/helpers/hash.c
    src/helpers/math.c
    tests/data_structures/hashmap_test.cpp
)
target_link_libraries(hashmap_test PRIVATE GTest::gtest_main)

target_include_directories(hashmap_test PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

gtest_discover_tests(hashmap_test)

add_executable(math_test
    src/helpers/math.c
    tests/helpers/math_test.cpp
)
target_link_libraries(math_test PRIVATE GTest::gtest_main)

target_include_directories(math_test PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

gtest_discover_tests(math_test)
