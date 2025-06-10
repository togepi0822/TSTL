cmake_minimum_required(VERSION 3.21)
project(TSTL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

option(TSTL_BUILD_TESTS "Build unit test" ${PROJECT_IS_TOP_LEVEL})

add_library(TSTL INTERFACE)

target_include_directories(TSTL INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (TSTL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif (TSTL_BUILD_TESTS)