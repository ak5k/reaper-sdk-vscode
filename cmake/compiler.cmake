include_guard(GLOBAL)

if(NOT TARGET ${PROJECT_NAME}_lib OR NOT TARGET ${PROJECT_NAME})
    message(
        FATAL_ERROR
        "Include cmake/compiler.cmake after targets are defined."
    )
endif()

target_compile_features(${PROJECT_NAME}_lib PUBLIC cxx_std_17)
# --- Warning flags (edit these to taste) ---
set(PROJECT_MSVC_WARNING_FLAGS
    /W3
    /WX
    /wd4244 # conversion, possible loss of data
    /wd4267 # size_t to int conversion
    /wd4305 # truncation from double to float
    /wd4996 # deprecated functions
)
set(PROJECT_GCC_CLANG_WARNING_FLAGS
    -Wall
    -Wextra
    -Werror
    -Wno-conversion
    -Wno-sign-conversion
)
# -------------------------------------------

if(MSVC)
    set(_compile_flags
        ${PROJECT_MSVC_WARNING_FLAGS}
        $<$<NOT:$<CONFIG:Debug>>:/O2
        /Ob3
        /Oi
        /Ot
        /fp:fast
        /arch:AVX2>
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
        -mavx2
        -mfma>
    )
endif()

target_compile_options(${PROJECT_NAME}_lib PUBLIC ${_compile_flags})
