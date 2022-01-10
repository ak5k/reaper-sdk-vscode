add_library(
    reaper_plugin OBJECT
    main.cpp
    my_plugin.cpp
)

target_include_directories(
  reaper_plugin SYSTEM PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/vendor 
  ${CMAKE_SOURCE_DIR}/vendor/reaper-sdk/sdk)

target_link_libraries(${PROJECT_NAME} reaper_plugin)

if(WIN32)
  target_compile_options(reaper_plugin PUBLIC
    /W3
    /WX
    )
   target_compile_definitions(reaper_plugin PUBLIC NOMINMAX)
else()
  target_compile_options(reaper_plugin PUBLIC
    -Wall -Wextra -Werror -Wpedantic
  )
endif()

find_package(WDL REQUIRED)
target_link_libraries(reaper_plugin WDL::WDL)

if(NOT WIN32)
  find_package(SWELL REQUIRED)
  target_link_libraries(reaper_plugin SWELL::swell)
endif()
