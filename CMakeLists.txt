project(forkmem LANGUAGES CXX)
cmake_minimum_required(VERSION 3.23)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake/Modules")

if (EXISTS "${CMAKE_SOURCE_DIR}/.boost_1_78_0/")
    set(BOOST_SOURCE_ROOT "./.boost_1_78_0/")
endif()
include(SetupBoost)

add_library(forkmem STATIC)
target_include_directories(forkmem PUBLIC include/)
target_sources(forkmem PUBLIC
    src/forkmem/unix/bridge.cxx
    src/forkmem/unix/IpcMemory.cxx
    src/forkmem/Atomic.cxx
)

target_link_libraries(forkmem PRIVATE Boost::atomic Boost::process)
if(UNIX AND NOT APPLE)
    target_link_libraries(forkmem PRIVATE rt dl)
endif()

add_library(forkmem-child INTERFACE)
target_sources(forkmem-child INTERFACE
    include/forkmem/child/Child.hxx
    src/forkmem/child/Child.cxx
)
target_link_libraries(forkmem-child INTERFACE forkmem)

add_library(ardrivo SHARED)
target_sources(ardrivo PUBLIC
    src/ardrivo/sketch_main.cxx
    src/ardrivo/lib/HardwareSerial.cxx
    src/ardrivo/lib/Print.cxx
    src/ardrivo/lib/Stream.cxx
    src/ardrivo/lib/String.cxx
    src/ardrivo/lib/WiFi.cxx
    src/ardrivo/lib/Arduino.cxx
)
target_include_directories(ardrivo PUBLIC include/)
target_link_libraries(ardrivo PUBLIC forkmem-child)


add_library(ssmce STATIC)
target_include_directories(ssmce PUBLIC include/ ${CMAKE_BINARY_DIR}/embed)
target_sources(ssmce PUBLIC
    src/ssmce/Sketch.cxx
    src/ssmce/Library.cxx
)
target_link_libraries(ssmce PUBLIC ardrivo forkmem)

function(embed_text input subdir output)
    file(READ "${input}" text)
    set(var_name "${output}")
    set(data "${text}")
    string(LENGTH "${text}" size)
    message("${CMAKE_BINARY_DIR}/${subdir}/${output}.hxx")
    configure_file("share/embed_header.hxx.in" "${CMAKE_BINARY_DIR}/${subdir}/${output}.hxx" @ONLY)
endfunction(embed_text)

set(embed_out "embed/cmake")
embed_text("share/cmake/CMakeLists.txt" ${embed_out} "CMakeLists")
embed_text("share/cmake/ConfigureSketch.cmake" ${embed_out} "ConfigureSketch")
embed_text("share/cmake/LibraryGen.cmake" ${embed_out} "LibraryGen")
embed_text("share/cmake/Preprocessing.cmake" ${embed_out} "Preprocessing")
embed_text("share/cmake/ProbeCompilerIncdirs.cmake" ${embed_out} "ProbeCompilerIncdirs")
embed_text("share/cmake/sketch_main.cxx" "embed/snippets" "SketchMain")

add_executable(forkmem-test)
target_sources(forkmem-test PUBLIC src/main.cxx)
target_link_libraries(forkmem-test PUBLIC ssmce)