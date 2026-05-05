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
# -------------------------------------------

if(MSVC)
    foreach(
        _var
        CMAKE_C_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_MINSIZEREL
    )
        string(REPLACE "/Ob2" "/Ob3" ${_var} "${${_var}}")
    endforeach()
    set(_compile_flags
        ${PROJECT_MSVC_WARNING_FLAGS} ##
        $<$<NOT:$<CONFIG:Debug>>:/Oi
        /Ot
        /fp:fast
        ${PROJECT_X86_SIMD_FLAGS}>
    )
else()
    set(_compile_flags
        ${PROJECT_GCC_CLANG_WARNING_FLAGS}
        $<$<NOT:$<CONFIG:Debug>>:-O3
        -fomit-frame-pointer
        -ffast-math
        -fno-math-errno
        -fno-trapping-math
        -ffunction-sections
        -fdata-sections
        ${PROJECT_X86_SIMD_FLAGS}>
    )
endif()

target_compile_options(${PROJECT_NAME}_lib PUBLIC ${_compile_flags})
