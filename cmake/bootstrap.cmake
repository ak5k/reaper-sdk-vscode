include_guard(GLOBAL)
include(FetchContent)

# Reuse already populated dependency sources across cache resets.
file(
    GLOB _fetchcontent_source_dirs
    LIST_DIRECTORIES true
    "${FETCHCONTENT_BASE_DIR}/*-src"
)
foreach(_source_dir IN LISTS _fetchcontent_source_dirs)
    cmake_path(GET _source_dir FILENAME _content_name)
    string(REGEX REPLACE "-src$" "" _content_name "${_content_name}")
    string(TOUPPER "${_content_name}" _content_name_upper)
    set("FETCHCONTENT_SOURCE_DIR_${_content_name_upper}"
        "${_source_dir}"
        CACHE PATH
        "Override ${_content_name} FetchContent source dir"
    )
endforeach()

# ── Project metadata ─────────────────────────────────────────────────────────

set(PROJECT_VENDOR "MyUsername")
set(_version "0.0.0")

set(PROJECT_VERSION_BUILD 0)
set(PROJECT_VERSION_COMMIT "unknown")
set(PROJECT_VERSION_BRANCH "unknown")
set(PROJECT_VERSION_TWEAK "0")

# return()

find_package(Git REQUIRED)

# ── Version from git tags ────────────────────────────────────────────────────
# Parses the most recent tag matching v?X.Y.Z[(-branch)][.tweak] to derive
# the project version. Falls back to 0.0.0 if no matching tag is found.

execute_process(
    COMMAND ${GIT_EXECUTABLE} tag --list --sort=-version:refname
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE _git_tags
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
string(REPLACE "\n" ";" _git_tags "${_git_tags}")
foreach(_git_tag IN LISTS _git_tags)
    if(_git_tag MATCHES "^v?[0-9]+\\.[0-9]+\\.[0-9]+")
        string(
            REGEX MATCH "^v?([0-9]+\\.[0-9]+\\.[0-9]+)"
            _ignored
            "${_git_tag}"
        )
        set(_version "${CMAKE_MATCH_1}")
        string(
            REGEX REPLACE "^v?[0-9]+\\.[0-9]+\\.[0-9]+"
            ""
            _remainder
            "${_git_tag}"
        )
        if(_remainder MATCHES "\\.[0-9]+$")
            string(REGEX MATCH "[0-9]+$" PROJECT_VERSION_TWEAK "${_remainder}")
        endif()
        break()
    endif()
endforeach()

set(_version "${_version}.${PROJECT_VERSION_TWEAK}")

if(DEFINED ENV{GITHUB_RUN_NUMBER} AND NOT "$ENV{GITHUB_RUN_NUMBER}" STREQUAL "")
    set(PROJECT_VERSION_BUILD "$ENV{GITHUB_RUN_NUMBER}")
endif()

if(DEFINED ENV{GITHUB_SHA} AND NOT "$ENV{GITHUB_SHA}" STREQUAL "")
    set(PROJECT_VERSION_COMMIT "$ENV{GITHUB_SHA}")
else()
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE _git_head_commit
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(NOT "${_git_head_commit}" STREQUAL "")
        set(PROJECT_VERSION_COMMIT "${_git_head_commit}")
    endif()
endif()

if(DEFINED ENV{GITHUB_REF_NAME} AND NOT "$ENV{GITHUB_REF_NAME}" STREQUAL "")
    set(PROJECT_VERSION_BRANCH "$ENV{GITHUB_REF_NAME}")
else()
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE _git_branch
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(NOT "${_git_branch}" STREQUAL "")
        set(PROJECT_VERSION_BRANCH "${_git_branch}")
    endif()
endif()

# ── Git remotes ──────────────────────────────────────────────────────────────

execute_process(
    COMMAND ${GIT_EXECUTABLE} remote get-url upstream
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE _upstream_url
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    RESULT_VARIABLE _upstream_exists
)
if(_upstream_exists EQUAL 0 AND _upstream_url MATCHES "justinfrankel")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} remote remove upstream
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_QUIET
        ERROR_QUIET
    )
endif()

# ── Git hooks ────────────────────────────────────────────────────────────────

if(
    EXISTS "${CMAKE_SOURCE_DIR}/.git"
    AND IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.githooks"
)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} config core.hooksPath .githooks
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_QUIET
        ERROR_QUIET
    )
endif()

# ── Build artifact staging ──────────────────────────────────────────────────

set(CMAKE_ARTIFACTS_DIR
    "${CMAKE_BINARY_DIR}/artifacts"
    CACHE PATH
    "Directory for staged build artifacts"
)
set(CMAKE_ARTIFACTS_BIN_DIR "${CMAKE_ARTIFACTS_DIR}/bin")
set(CMAKE_ARTIFACTS_SYMBOLS_DIR "${CMAKE_ARTIFACTS_DIR}/symbols")

file(REMOVE_RECURSE "${CMAKE_ARTIFACTS_DIR}")
file(MAKE_DIRECTORY "${CMAKE_ARTIFACTS_BIN_DIR}" "${CMAKE_ARTIFACTS_SYMBOLS_DIR}")

function(configure_target_artifact_output target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "Target '${target_name}' does not exist.")
    endif()

    set_target_properties(
        ${target_name}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_ARTIFACTS_BIN_DIR}"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_ARTIFACTS_BIN_DIR}"
    )

    if(MSVC)
        set_target_properties(
            ${target_name}
            PROPERTIES
                PDB_OUTPUT_DIRECTORY "${CMAKE_ARTIFACTS_SYMBOLS_DIR}"
                COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_ARTIFACTS_SYMBOLS_DIR}"
        )
    endif()
endfunction()
