cmake_minimum_required(VERSION 3.20)

project(xbutler-sync VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(FetchContent)

FetchContent_Declare(httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)
FetchContent_Declare(CLI
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.6.2
)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.17.0
)
FetchContent_Declare(
  curl
  GIT_REPOSITORY https://github.com/curl/curl.git
  GIT_TAG curl-8_14_1
)
FetchContent_MakeAvailable(httplib CLI spdlog curl)

add_executable(xbutler-sync
    src/main.cpp
    src/cli/cli.cpp
    src/server/server.cpp
    src/client/client.cpp
)
target_include_directories(xbutler-sync PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(xbutler-sync httplib::httplib CLI11::CLI11 spdlog::spdlog CURL::libcurl)


find_program(CLANG_FORMAT_BIN NAMES clang-format)
if(CLANG_FORMAT_BIN)
  add_custom_target(
    format
    COMMAND
      ${CLANG_FORMAT_BIN} -i
      ${PROJECT_SOURCE_DIR}/src/main.cpp
      ${PROJECT_SOURCE_DIR}/src/cli/cli.h
      ${PROJECT_SOURCE_DIR}/src/cli/cli.cpp
      ${PROJECT_SOURCE_DIR}/src/server/server.h
      ${PROJECT_SOURCE_DIR}/src/server/server.cpp
      ${PROJECT_SOURCE_DIR}/src/client/client.h
      ${PROJECT_SOURCE_DIR}/src/client/client.cpp
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
      ${PROJECT_SOURCE_DIR}/src/cli/cli.cpp
      ${PROJECT_SOURCE_DIR}/src/server/server.cpp
      ${PROJECT_SOURCE_DIR}/src/client/client.cpp
      --
      -I${PROJECT_SOURCE_DIR}/include
      -I${PROJECT_SOURCE_DIR}/src
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
