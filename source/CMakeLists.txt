add_library(
    reaper_plugin OBJECT
    main.cpp
    my_plugin.cpp
)

find_package(WDL REQUIRED)
target_link_libraries(reaper_plugin WDL::WDL)

if(NOT WIN32)
  find_package(SWELL REQUIRED)
  target_link_libraries(reaper_plugin SWELL::swell)
endif()

target_link_libraries(${PROJECT_NAME} reaper_plugin)

if(WIN32)
  target_compile_options(reaper_plugin PUBLIC
    /W3
    /WX
    /wd5208
    )
  # target_compile_definitions(reaper_plugin PUBLIC NOMINMAX UNICODE)
else()
  target_compile_options(reaper_plugin PUBLIC
    -Wall -Wextra -Werror -Wpedantic
    # -Wno-unused-parameter -Wno-missing-field-initializers
    # -Wno-unused-function -Wno-missing-braces
  )
endif()
