cmake_minimum_required(VERSION 4.0)

include(cmake/prepare.cmake)

project(MyPlugin VERSION ${_version} LANGUAGES C CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME}_lib STATIC)
add_library(${PROJECT_NAME} SHARED)

# Configure config.h from config.h.in
configure_file(
    "${CMAKE_SOURCE_DIR}/src/config.h.in"
    "${CMAKE_BINARY_DIR}/config.h"
)
target_include_directories(
    ${PROJECT_NAME}_lib
    SYSTEM
    PUBLIC ${CMAKE_BINARY_DIR}
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/reaper-plugin.cmake)
include(cmake/tests.cmake)
include(cmake/clang-tidy.cmake)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "_test\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX "main\\.cpp$")
target_sources(${PROJECT_NAME}_lib PRIVATE ${SOURCES})
target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src/main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

if(MSVC)
    target_compile_options(
        ${PROJECT_NAME}_lib
        PUBLIC /W3 /WX /wd4996 /wd4244 /wd4267 /wd4305
    )
else()
    target_compile_options(
        ${PROJECT_NAME}_lib
        PUBLIC -Wall -Wextra -Werror -Wno-conversion -Wno-sign-conversion
    )
endif()

include(cmake/code-signing.cmake)
include(cmake/packaging.cmake)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}" --config $<CONFIG>
    COMMENT "Installing ${PROJECT_NAME}..."
    VERBATIM
)
