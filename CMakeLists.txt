cmake_minimum_required(VERSION 3.25)

project(Strazzle)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/main.cpp")


add_executable("${PROJECT_NAME}"
    "${SOURCES}"
)


target_include_directories("${PROJECT_NAME}" PRIVATE
    "include"
)