cmake_minimum_required(VERSION 3.20)

project(xbutler-sync VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(xbutler-sync src/main.cpp)
target_include_directories(xbutler-sync PRIVATE ${PROJECT_SOURCE_DIR}/include)

find_program(CLANG_FORMAT_BIN NAMES clang-format)
if(CLANG_FORMAT_BIN)
  add_custom_target(
    format
    COMMAND
      ${CLANG_FORMAT_BIN} -i
      ${PROJECT_SOURCE_DIR}/src/main.cpp
      ${PROJECT_SOURCE_DIR}/tests/smoke_test.cpp
    VERBATIM)
else()
  add_custom_target(
    format
    COMMAND ${CMAKE_COMMAND} -E echo "clang-format not found; skipping format target"
    VERBATIM)
endif()

find_program(CLANG_TIDY_BIN NAMES clang-tidy)
if(CLANG_TIDY_BIN)
  add_custom_target(
    lint
    COMMAND
      ${CLANG_TIDY_BIN}
      ${PROJECT_SOURCE_DIR}/src/main.cpp
      --
      -I${PROJECT_SOURCE_DIR}/include
      -std=c++23
    VERBATIM)
else()
  add_custom_target(
    lint
    COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy not found; skipping lint target"
    VERBATIM)
endif()

enable_testing()
add_subdirectory(tests)
