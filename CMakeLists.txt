cmake_minimum_required(VERSION 3.31)
project(order-book-programming-problem)

set(CMAKE_CXX_STANDARD 23)
option(INCLUDE_TEST "Enable building test binaries" 0)
option(BENCHMARK_PERFORMANCE "Enable performance benchmarking" 0)

add_compile_definitions(BENCHMARK_PERFORMANCE=${BENCHMARK_PERFORMANCE})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")

if (INCLUDE_TEST)
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

    add_executable(order-test src/tests/order-test.cpp)
    target_link_libraries(order-test utils GTest::gtest GTest::gtest_main GTest::gmock)

endif ()

add_library(utils src/utils.cpp)


add_executable(pricer-array src/pricer.cpp)
target_compile_definitions(pricer-array PRIVATE ORDER_BOOK_IMPL_ARRAY=1)
target_link_libraries(pricer-array utils)

add_executable(pricer-bst src/pricer.cpp)
target_compile_definitions(pricer-bst PRIVATE ORDER_BOOK_IMPL_BST=1)
target_link_libraries(pricer-bst utils)

add_executable(pricer-std-map src/pricer.cpp)
target_compile_definitions(pricer-std-map PRIVATE ORDER_BOOK_IMPL_STD_MAP=1)
target_link_libraries(pricer-std-map utils)
