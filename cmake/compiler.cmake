include_guard(GLOBAL)

if(NOT TARGET ${PROJECT_NAME}_lib OR NOT TARGET ${PROJECT_NAME})
    message(
        FATAL_ERROR
        "Include cmake/compiler.cmake after targets are defined."
    )
endif()

# --- Warning flags (edit these to taste) ---
set(PROJECT_MSVC_WARNING_FLAGS
    /W3
    /WX
    /wd4244 # conversion, possible loss of data
    /wd4267 # size_t to int conversion
    /wd4305 # truncation from double to float
    /wd4714 # function marked as __forceinline not inlined
    /wd4996 # deprecated functions
)
set(PROJECT_GCC_CLANG_WARNING_FLAGS
    -Wall
    -Wextra
    -Werror
    -Wno-conversion
    -Wno-sign-conversion
)

set(PROJECT_X86_SIMD_FLAGS)
string(TOLOWER "${CMAKE_CXX_COMPILER_TARGET}" _compiler_target_lower)
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _processor_lower)
if(
    NOT _compiler_target_lower MATCHES "^(arm.*|aarch64)"
    AND NOT _processor_lower MATCHES "^(arm.*|aarch64)$"
)
    if(MSVC)
        set(PROJECT_X86_SIMD_FLAGS /arch:AVX2)
    else()
        set(PROJECT_X86_SIMD_FLAGS -mavx2 -mfma)
    endif()
endif()

if(MSVC)
    foreach(
        _flags_var
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS_MINSIZEREL
    )
        if(${_flags_var} MATCHES "/Ob2")
            string(REPLACE "/Ob2" "/Ob3" ${_flags_var} "${${_flags_var}}")
            set(${_flags_var} "${${_flags_var}}" CACHE STRING "" FORCE)
        endif()
    endforeach()

    set(_compile_flags
        ${PROJECT_MSVC_WARNING_FLAGS}
        $<$<NOT:$<CONFIG:Debug>>:/Oi
        /GF
        /fp:fast
        ${PROJECT_X86_SIMD_FLAGS}>
    )
else()
    set(_compile_flags
        ${PROJECT_GCC_CLANG_WARNING_FLAGS}
        $<$<NOT:$<CONFIG:Debug>>:-O3
        -ffast-math
        -ffunction-sections
        -fdata-sections
        ${PROJECT_X86_SIMD_FLAGS}>
    )
endif()

if(MSVC)
    set(_link_flags $<$<NOT:$<CONFIG:Debug>>:/OPT:REF /OPT:ICF>)
elseif(APPLE)
    set(_link_flags $<$<NOT:$<CONFIG:Debug>>:-Wl,-dead_strip>)
else()
    set(_link_flags $<$<NOT:$<CONFIG:Debug>>:-Wl,--gc-sections>)
endif()

foreach(_target ${PROJECT_NAME} ${PROJECT_NAME}_lib)
    set_target_properties(
        ${_target}
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
            INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE
            INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE
    )
endforeach()

target_link_options(${PROJECT_NAME} PRIVATE ${_link_flags})
target_compile_options(${PROJECT_NAME}_lib PRIVATE ${_compile_flags})
target_compile_options(${PROJECT_NAME} PRIVATE ${_compile_flags})
