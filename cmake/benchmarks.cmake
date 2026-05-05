set(_benchmark_sources ${SOURCES})
list(FILTER _benchmark_sources INCLUDE REGEX "_bench\\.cpp$")
list(FILTER SOURCES EXCLUDE REGEX "_bench\\.cpp$")

if(DEFINED ENV{CI} AND NOT ENABLE_BENCHMARK_TESTS)
    message(
        STATUS
        "Skipping benchmarks in CI because ENABLE_BENCHMARK_TESTS is OFF"
    )
    return()
endif()

if(NOT _benchmark_sources)
    return()
endif()

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark
    GIT_TAG main
    SYSTEM
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

if(WIN32)
    if(TARGET benchmark)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(benchmark PRIVATE -w -Wno-c2y-extensions)
        endif()
        set_property(
            TARGET benchmark
            PROPERTY
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
    endif()
    if(TARGET benchmark_main)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(
                benchmark_main
                PRIVATE -w -Wno-c2y-extensions
            )
        endif()
        set_property(
            TARGET benchmark_main
            PROPERTY
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
    endif()
endif()

add_executable(${PROJECT_NAME}_benchmarks ${_benchmark_sources})
configure_target_artifact_output(${PROJECT_NAME}_benchmarks)
if(WIN32)
    set_property(
        TARGET ${PROJECT_NAME}_benchmarks
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
endif()

if(ENABLE_BENCHMARK_TESTS AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(${PROJECT_NAME}_lib PRIVATE -g)
    target_compile_options(${PROJECT_NAME}_benchmarks PRIVATE -g)
endif()

target_link_libraries(
    ${PROJECT_NAME}_benchmarks
    PRIVATE ${PROJECT_NAME}_lib benchmark::benchmark
)

set(_benchmark_output_dir "${CMAKE_BINARY_DIR}/benchmarks")
file(MAKE_DIRECTORY "${_benchmark_output_dir}")
string(TIMESTAMP _benchmark_timestamp "%Y%m%d-%H%M%S")

if(NOT DEFINED ENV{CI} OR ENABLE_BENCHMARK_TESTS)
    set(_benchmark_command
        $<TARGET_FILE:${PROJECT_NAME}_benchmarks>
        --benchmark_out=${_benchmark_output_dir}/benchmark-${_benchmark_timestamp}.json
    )

    if(ENABLE_BENCHMARK_TESTS AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
        find_program(_perf_program perf)
        if(NOT _perf_program)
            message(
                FATAL_ERROR
                "ENABLE_BENCHMARK_TESTS is ON but 'perf' was not found on PATH. Install perf and reconfigure."
            )
        endif()

        set(_benchmark_command
            ${_perf_program}
            record
            --call-graph=dwarf
            --no-buildid
            --no-buildid-mmap
            --output=${_benchmark_output_dir}/perf-${_benchmark_timestamp}.data
            --
            $<TARGET_FILE:${PROJECT_NAME}_benchmarks>
            --benchmark_out=${_benchmark_output_dir}/benchmark-${_benchmark_timestamp}.json
        )
    endif()

    add_test(NAME run_benchmarks COMMAND ${_benchmark_command})
    set_tests_properties(
        run_benchmarks
        PROPERTIES LABELS benchmark FIXTURES_SETUP benchmark_perf_data
    )

    if(ENABLE_BENCHMARK_TESTS AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_test(
            NAME generate_perf_report
            COMMAND
                bash -c
                "${_perf_program} report \
                    --stdio \
                    --no-children \
                    --percent-limit=1 \
                    --input=${_benchmark_output_dir}/perf-${_benchmark_timestamp}.data \
                    > ${_benchmark_output_dir}/perf-report-${_benchmark_timestamp}.txt"
        )
        set_tests_properties(
            generate_perf_report
            PROPERTIES LABELS benchmark FIXTURES_REQUIRED benchmark_perf_data
        )
    endif()
endif()
