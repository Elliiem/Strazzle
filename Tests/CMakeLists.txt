cmake_minimum_required(VERSION 3.25)

project(Strazzle-Test)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories("${CMAKE_SOURCE_DIR}/include")

file(GLOB TEST_SOURCES "${CMAKE_SOURCE_DIR}/Tests/*.cpp")

add_executable(Tests
    "${TEST_SOURCES}"
)

target_link_libraries(Tests ${GTEST_BOTH_LIBRARIES} pthread)

add_custom_target(test COMMAND "${CMAKE_BINARY_DIR}/Tests/Tests" DEPENDS Tests)
