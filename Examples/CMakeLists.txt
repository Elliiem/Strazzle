cmake_minimum_required(VERSION 3.25)

project(Strazzle-Examples)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE EXAMPLE_DIRECTORIES LIST_DIRECTORIES ON "${CMAKE_SOURCE_DIR}/Examples/**")

foreach(DIR ${EXAMPLE_DIRECTORIES})
    if(NOT IS_DIRECTORY "${DIR}")
        remove(EXAMPLE_DIRECTORIES ${DIR})
    endif(NOT IS_DIRECTORY "${DIR}")
endforeach(DIR ${EXAMPLE_DIRECTORIES})

include_directories("${CMAKE_SOURCE_DIR}/include")

foreach(DIR ${EXAMPLE_DIRECTORIES})
    file(GLOB EXAMPLE_SOURCES "${DIR}/*.cpp")
    foreach(SOURCE ${EXAMPLE_SOURCES})
        get_filename_component(EXECUTABLE_NAME "${SOURCE}" NAME_WE)

        add_executable("${EXECUTABLE_NAME}"
            ${SOURCE}
        )

        get_filename_component(DIRECTORY_NAME "${DIR}" NAME)
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Examples/${DIRECTORY_NAME}/")
        set_target_properties("${EXECUTABLE_NAME}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Examples/${DIRECTORY_NAME}/")
    endforeach(SOURCE ${EXAMPLE_SOURCES})
endforeach(DIR ${EXAMPLE_DIRECTORIES})