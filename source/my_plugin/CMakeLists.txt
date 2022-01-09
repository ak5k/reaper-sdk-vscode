# add my_plugin (target)
# as plain OBJECT

if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    link_libraries(swell)
endif()


add_library(
    my_plugin OBJECT
    # my plugin source files 
    my_plugin.cpp
)

