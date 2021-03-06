# some initial settings
cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# the project name and other information
# initial version
project(
    my_reaper_plugin
    VERSION 0.1.0
    LANGUAGES CXX
)

# set up reaper-sdk and WDL
# perhaps not best practice
# but for convinience
# if not setup
# gets reaper-sdk and WDL via git
# and symlinks WDL to reaper-sdk
if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/vendor/reaper-sdk/WDL)
    file(REMOVE ${CMAKE_CURRENT_LIST_DIR}/vendor/reaper-sdk/WDL)
    find_package(Git)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
    file(CREATE_LINK 
        ${CMAKE_CURRENT_LIST_DIR}/vendor/WDL/WDL
        ${CMAKE_CURRENT_LIST_DIR}/vendor/reaper-sdk/WDL 
        SYMBOLIC)
endif()

# set up headers
# reaper-sdk headers
# reasciprt_vararg.hpp (credits to cfillion)
# and everything else in ./include/
add_library(reaper_interface INTERFACE)
target_include_directories(
    reaper_interface SYSTEM INTERFACE
    ${PROJECT_SOURCE_DIR}/vendor/reaper-sdk/sdk
    ${PROJECT_SOURCE_DIR}/include
)
# all targets will 'have' reaper_interface
link_libraries(reaper_interface)

# avoid certain warning during build
if (CMAKE_SYSTEM_NAME MATCHES Windows)
    add_compile_options(/W3 /wd5208)
endif()

# set .dylib file extension on Mac
if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    set(CMAKE_SHARED_MODULE_SUFFIX ".dylib")
endif()

# some *nix settings
if (UNIX)
    add_compile_options(-Wall -Werror -Wpedantic -Wextra)
    if (NOT APPLE)
        add_compile_options(-shared -fPIC)
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
        set(BUILD_SHARED_LIBS ON)
    endif()
endif()


# add source files of plugin (the actual plugin itself)
# see CMakeList.txt under ./source/
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/source)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)