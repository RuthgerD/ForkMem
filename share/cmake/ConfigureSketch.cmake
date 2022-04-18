#
#  ConfigureSketch.cmake
#  Copyright 2021-2022 ItJustWorksTM
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

cmake_policy (SET CMP0011 NEW)

# CMake info
message ("CMAKE_VERSION: ${CMAKE_VERSION}")

# System info
unset (CMAKE_BINARY_DIR)
include (CMakeDetermineSystem)
message ("CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM}")
message ("CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set (MODULES_DIR "${SKETCH_BUILD_PATH}/CMake/Modules")
list (APPEND CMAKE_MODULE_PATH "${MODULES_DIR}")

# Find arduino-prelude or obtain it
find_program (ARDPRE_EXECUTABLE arduino-prelude)
if (NOT ARDPRE_EXECUTABLE)
  message (FATAL_ERROR "Could not find arduino-prelude")
endif ()

#begin ArduinoPreludeVersion
execute_process (COMMAND "${ARDPRE_EXECUTABLE}" --version
    RESULT_VARIABLE ARDPRE_VERSION_COMMAND_RESULT
    OUTPUT_VARIABLE ARDPRE_VERSION_COMMAND_OUTPUT
)
if (ARDPRE_VERSION_COMMAND_RESULT)
  message (FATAL_ERROR "Query for arduino-prelude version failed (${ARDPRE_VERSION_COMMAND_RESULT})")
endif ()

if ("${ARDPRE_VERSION_COMMAND_OUTPUT}" MATCHES "^arduino-prelude v([0-9]+\\.[0-9]+\\.[0-9]+)\n\r?$")
  set (ARDPRE_VERSION "${CMAKE_MATCH_1}")
  message (STATUS "Found arduino-prelude v${ARDPRE_VERSION}")
else ()
  message (FATAL_ERROR "Unable to determine arduino-prelude version:\n${ARDPRE_VERSION_COMMAND_OUTPUT}")
endif ()
#end ArduinoPreludeVersion

file (MAKE_DIRECTORY "${SKETCH_BUILD_PATH}/build")

execute_process (COMMAND "${CMAKE_COMMAND}" "-DARDPRE_EXECUTABLE=${ARDPRE_EXECUTABLE}" "-DSKETCH_DIR=${SKETCH_PATH}" -S "${SKETCH_BUILD_PATH}" -B "${SKETCH_BUILD_PATH}/build")
execute_process (COMMAND "${CMAKE_COMMAND}" "--build" "${SKETCH_BUILD_PATH}/build" "--config Release")

