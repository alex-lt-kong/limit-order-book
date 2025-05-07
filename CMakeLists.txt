cmake_minimum_required(VERSION 3.31)
project(binary_search_tree)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

################################
# GTest
################################
enable_testing()
find_package(GTest CONFIG REQUIRED COMPONENTS GTest GMock)
include_directories(${GTEST_INCLUDE_DIRS})

################################
# Unit Tests
################################
add_executable(bst-impl-test src/tests/bst-impl-test.cpp)
target_link_libraries(bst-impl-test GTest::gtest GTest::gtest_main GTest::gmock)

add_executable(heap-impl-test src/tests/heap-impl-test.cpp)
target_link_libraries(heap-impl-test GTest::gtest GTest::gtest_main GTest::gmock)

add_library(utils src/utils.cpp)

add_executable(order-test src/tests/order-test.cpp)
target_link_libraries(order-test utils GTest::gtest GTest::gtest_main GTest::gmock)

add_executable(pricer src/pricer.cpp)
target_link_libraries(pricer utils)