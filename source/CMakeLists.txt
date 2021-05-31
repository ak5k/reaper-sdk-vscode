# create reaper_plugin (target)
# add main source file
# reaper_plugin is our "main target"
add_library(
    reaper_plugin SHARED
    main.cpp
)

target_link_libraries(
    reaper_plugin
    reaper_interface
)

# add my plugin source
# to reaper_plugin build
# see CMakeLists.txt in ./my_plugin/
add_subdirectory(my_plugin)

# set swell for reaper_plugin
# if not Windows
if (NOT MSVC)
    add_library(swell ${CMAKE_SOURCE_DIR}/vendor/WDL/WDL/swell/swell-modstub$<IF:$<BOOL:${APPLE}>,.mm,-generic.cpp>)
    target_compile_definitions(swell PUBLIC SWELL_PROVIDED_BY_APP)
    target_link_libraries(reaper_plugin swell)
    if (APPLE)
        find_library(APPKIT_LIB AppKit)
        target_link_libraries(swell ${APPKIT_LIB})
    endif()
endif()

# build reaper_plugin directly into REAPER UserPlugins
# for convinience
if (MSVC)
    set_target_properties(reaper_plugin PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG $ENV{APPDATA}/REAPER/UserPlugins)
    set_target_properties(reaper_plugin PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE $ENV{APPDATA}/REAPER/UserPlugins)
    set_target_properties(reaper_plugin PROPERTIES RUNTIME_OUTPUT_DIRECTORY $ENV{APPDATA}/REAPER/UserPlugins)
elseif (APPLE)
    set_target_properties(reaper_plugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ~/Library/Application\ Support/REAPER/UserPlugins)
elseif (UNIX)
    set_target_properties(reaper_plugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ~/.config/REAPER/UserPlugins)
endif()

# set filename
# clear prefix
# filename has to be reaper_something
# for REAPER to load it
set_target_properties(reaper_plugin PROPERTIES PREFIX "")