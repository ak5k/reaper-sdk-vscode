cmake_minimum_required(VERSION 3.31)

include(cmake/bootstrap.cmake)

project(MyPlugin VERSION ${_version} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} SHARED)
# add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src/main.cpp")
configure_target_artifact_output(${PROJECT_NAME})

# core
add_library(${PROJECT_NAME}_lib STATIC)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "main\\.cpp$")
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/reaper-plugin.cmake)
include(cmake/compiler.cmake)
include(cmake/clang-tidy.cmake)
include(cmake/packaging.cmake)
include(cmake/code-signing.cmake)
include(cmake/tests.cmake)
include(cmake/benchmarks.cmake)

target_sources(${PROJECT_NAME}_lib PRIVATE ${SOURCES})
target_include_directories(
    ${PROJECT_NAME}_lib
    SYSTEM
    PUBLIC ${CMAKE_BINARY_DIR}
)

if(EXISTS "${CMAKE_SOURCE_DIR}/src/config.h.in")
    message(STATUS "Configuring config.h from config.h.in")
    configure_file(
        "${CMAKE_SOURCE_DIR}/src/config.h.in"
        "${CMAKE_BINARY_DIR}/config.h"
    )
endif()

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --config $<CONFIG>
    COMMENT "Installing ${PROJECT_NAME}..."
    VERBATIM
)
