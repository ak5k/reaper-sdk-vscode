# add my_plugin
target_sources(
    reaper_plugin PRIVATE
    # my plugin source files from current directory
    ${CMAKE_CURRENT_LIST_DIR}/my_plugin # .cpp, .hpp etc
)