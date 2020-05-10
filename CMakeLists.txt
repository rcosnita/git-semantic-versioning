cmake_minimum_required(VERSION 3.17)
set(CMAKE_GENERATOR make)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "-Wc++11-extensions -std=c++2a")
enable_language(CXX)

project(git-semantic-versioning)
include_directories(include lib/libgit2/include lib/spdlog/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

file(GLOB SOURCES "src/*.cpp" "src/**/*.cpp")

add_subdirectory(lib/libgit2)
add_subdirectory(lib/spdlog)
add_executable(git_semver ${SOURCES})
target_link_libraries(git_semver git2 spdlog)
set(TARGET git_semver PROPERTY CXX_STANDARD 20)