cmake_minimum_required(VERSION 3.30)
project(ophanim-down LANGUAGES CXX)

# Set C++20 standard with required support
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable compiler-specific extensions

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release flags" FORCE)

#  Use modern CMake policies
cmake_policy(SET CMP0077 NEW)  # Honor COMPILE_LANGUAGE for target_sources()
include(FetchContent)

# Check and fetch CPR
find_package(cpr QUIET)
if (NOT cpr_FOUND)
    message("Fetching dependency from github: cpr")
    FetchContent_Declare(
            cpr
            GIT_REPOSITORY https://github.com/libcpr/cpr.git
            GIT_TAG 1.9.9
    )
    FetchContent_MakeAvailable(cpr)
endif ()

# Check and fetch spdlog
find_package(spdlog QUIET)
if (NOT spdlog_FOUND)
    message("Fetching dependency from github: spdlog")
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.15.1
    )
    # Avoid building spdlog tests and examples
    set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "")
    set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "")
    FetchContent_MakeAvailable(spdlog)
endif ()

# Check and fetch nlohmann_json
find_package(nlohmann_json QUIET)
if (NOT nlohmann_json_FOUND)
    message("Fetching dependency from github: nlohmann-json")
    FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif ()

# We won't bother to download this one
find_library(SQLITE3_LIBRARY sqlite3 REQUIRED)

# Add executable
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/vpn/*.cpp" "src/video_manager/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        cpr::cpr
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        ${SQLITE3_LIBRARY}
)

#  Warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
