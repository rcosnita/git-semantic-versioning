cmake_minimum_required(VERSION 3.17)
set(CMAKE_GENERATOR make)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-Wc++11-extensions -std=c++2a")
enable_language(CXX)

project(git-semantic-versioning)
include_directories(include lib/libgit2/include lib/spdlog/include build/lib/gflags/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

file(GLOB SOURCES "src/*.cpp" "src/**/*.cpp")

if (CMAKE_BUILD_TYPE STREQUAL release)
    set(BUILD_SHARED_LIBS OFF)
endif (CMAKE_BUILD_TYPE STREQUAL release)
add_subdirectory(lib/libgit2)

if (CMAKE_BUILD_TYPE STREQUAL release)
    set(SPDLOG_BUILD_SHARED OFF)
endif (CMAKE_BUILD_TYPE STREQUAL release)
add_subdirectory(lib/spdlog)

set(GFLAGS_BUILD_gflags_nothreads_LIB OFF)
set(BUILD_gflags_LIB ON)
if (CMAKE_BUILD_TYPE STREQUAL release)
    set(GFLAGS_BUILD_SHARED_LIBS OFF)
    set(GFLAGS_BUILD_STATIC_LIBS ON)
endif (CMAKE_BUILD_TYPE STREQUAL release)
add_subdirectory(lib/gflags)

add_executable(git_semver ${SOURCES})
target_link_libraries(git_semver git2 spdlog gflags)
set(TARGET git_semver PROPERTY CXX_STANDARD 20)