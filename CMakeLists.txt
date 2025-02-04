cmake_minimum_required(VERSION 3.30)
project(ophanim-down LANGUAGES CXX)

# Set C++20 standard with required support
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable compiler-specific extensions

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Release flags" FORCE)

#  Use modern CMake policies
cmake_policy(SET CMP0077 NEW)  # Honor COMPILE_LANGUAGE for target_sources()

find_package(cpr REQUIRED)

# Add executable
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)

#  Warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
