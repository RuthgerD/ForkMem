
project(Sketch)
cmake_minimum_required(VERSION 3.23)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules")

# begin UseHighestCxxStandard
foreach (CXX_COMPILE_FEATURE ${CMAKE_CXX_COMPILE_FEATURES})
  if (CXX_COMPILE_FEATURE MATCHES "cxx_std_([0-8][0-9]+)")
    list (APPEND CXX_SUPPORTED_VERSIONS "${CMAKE_MATCH_1}")
  endif ()
endforeach ()
list (SORT CXX_SUPPORTED_VERSIONS)
list (REVERSE CXX_SUPPORTED_VERSIONS)
list (GET CXX_SUPPORTED_VERSIONS 1 CXX_HIGHEST_SUPPORTED_VERSION)
set (CMAKE_CXX_STANDARD "${CXX_HIGHEST_SUPPORTED_VERSION}")
message (STATUS "Using CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
# end UseHighestCxxStandard

add_library(Sketch SHARED)
target_sources(Sketch PRIVATE "sketch_main.cxx")
target_include_directories(Sketch PRIVATE ${SKETCH_DIR})
file (GLOB CXX_SOURCES LIST_DIRECTORIES false "${SKETCH_DIR}/*.cpp" "${SKETCH_DIR}/*.cxx" "${SKETCH_DIR}/*.cc" "${SKETCH_DIR}/*.c++")
target_sources(Sketch PRIVATE ${CXX_SOURCES})

add_custom_command(TARGET Sketch POST_BUILD COMMAND "${CMAKE_COMMAND}" -E rename "$<TARGET_FILE:Sketch>" "${PROJECT_BINARY_DIR}/Sketch")

include(Libraries)
include(Preprocessing)


