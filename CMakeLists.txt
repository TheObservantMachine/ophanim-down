cmake_minimum_required(VERSION 3.30)
project(ophanim-down LANGUAGES CXX)

# Set C++20 standard with required support
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable compiler-specific extensions

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release flags" FORCE)

#  Use modern CMake policies
cmake_policy(SET CMP0077 NEW)  # Honor COMPILE_LANGUAGE for target_sources()

# Check for <format> support 
include(CheckIncludeFileCXX)
check_include_file_cxx("format" HAS_STD_FORMAT)
if(NOT HAS_STD_FORMAT)
  message(STATUS "Using libfmt as a fallback for <format>")
  find_package(fmt REQUIRED)
endif()


find_package(CURL REQUIRED) 
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_library(SQLITE3_LIBRARY sqlite3 REQUIRED)

# Add executable
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/vpn/*.cpp" "src/video_manager/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(${CURL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        ${SQLITE3_LIBRARY}
        CURL::libcurl
)

# Link against libfmt if std::format isn’t available.
if(NOT HAS_STD_FORMAT)
  target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
endif()

#  Warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
