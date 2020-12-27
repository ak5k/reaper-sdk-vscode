cmake_minimum_required(VERSION 3.16.3)

################################################################################

# change "ak5k: my plugin" to something
# ACTION_NAME, COMMAND_NAME and FILENAME
# are parsed from this
set(PLUGIN_NAME "ak5k: my plugin")

################################################################################


project(plugin)
add_library(plugin SHARED 
    plugin.cpp
    #other libraries here
    )


# some atomic platform dependent configuration
if (MSVC)
    set_target_properties(plugin PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG $ENV{APPDATA}/REAPER/UserPlugins)
    set_target_properties(plugin PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE $ENV{APPDATA}/REAPER/UserPlugins)
    set_target_properties(plugin PROPERTIES RUNTIME_OUTPUT_DIRECTORY $ENV{APPDATA}/REAPER/UserPlugins)
elseif (APPLE)
    execute_process(COMMAND ${PROJECT_SOURCE_DIR}/configure_mac.sh)
    set_target_properties(plugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ~/Library/Application\ Support/REAPER/UserPlugins)
    target_sources(plugin PUBLIC ../../WDL/swell/swell-modstub.mm) 
    target_link_libraries(plugin "-framework Cocoa")
elseif (UNIX)
    set_target_properties(plugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ~/.config/REAPER/UserPlugins)
    target_sources(plugin PUBLIC ../../WDL/swell/swell-modstub-generic.cpp) 
endif()

if (APPLE OR UNIX)
    target_compile_definitions(plugin PUBLIC SWELL_PROVIDED_BY_APP)
endif()


# sets name of reaper_plugin
# by parsing PLUGIN_NAME
set(ACTION_NAME ${PLUGIN_NAME})
set(COMMAND_NAME ${PLUGIN_NAME})
string(REPLACE " " "_" COMMAND_NAME ${COMMAND_NAME})
string(REGEX REPLACE "[^A-Za-z0-9_]" "" COMMAND_NAME ${COMMAND_NAME})
string(TOUPPER ${COMMAND_NAME} COMMAND_NAME)

# configures config.h header file
configure_file(config.h.in config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

# replace spaces with dashes in filename
# strip colons from filename
# set filename to lowercase
set(FILENAME ${PLUGIN_NAME})
string(REPLACE " " "-" FILENAME ${FILENAME})
string(REGEX REPLACE "[^A-Za-z0-9_-]" "" FILENAME ${FILENAME})
string(TOLOWER ${FILENAME} FILENAME)

# set filename
# reaper_ prefix needed
set_target_properties(plugin PROPERTIES PREFIX "reaper_")
set_target_properties(plugin PROPERTIES OUTPUT_NAME ${FILENAME})