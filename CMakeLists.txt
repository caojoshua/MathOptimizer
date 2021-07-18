cmake_minimum_required(VERSION 3.10)

project(MathOptimizer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(src)

target_include_directories(mathopt PUBLIC include)

# Formatting target
FILE(GLOB_RECURSE
  ALL_CXX_SOURCE_FILES
  *.cpp *.h)

find_program(CLANG_FORMAT "clang-format")
if (CLANG_FORMAT)
  add_custom_target(format
    COMMAND clang-format
    -i
    -style=LLVM
    ${ALL_CXX_SOURCE_FILES})
endif()
