cmake_minimum_required(VERSION 3.17)
project (forkmem VERSION 0.1 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake/Modules")


if (EXISTS "${CMAKE_SOURCE_DIR}/.boost/")
    set(BOOST_SOURCE_ROOT "./.boost/")
endif()
set(BOOST_INCLUDE_LIBRARIES atomic container process)
include(SetupBoost)

add_library(forkmem STATIC)
target_include_directories(forkmem PUBLIC include/)
target_sources(forkmem PRIVATE
    src/forkmem/sync/Atomic.cxx
    src/forkmem/sync/Mutex.cxx
    src/forkmem/sync/RecursiveMutex.cxx
    
    src/forkmem/PoolResource.cxx
    src/forkmem/Memory.cxx
    src/forkmem/detail/UnixMemory.cxx
    src/forkmem/detail/Win32Memory.cxx

)

target_link_libraries(forkmem PUBLIC Boost::atomic Boost::container Boost::process)

if(UNIX AND NOT APPLE)
    target_link_libraries(forkmem PUBLIC rt dl)
endif()

enable_testing()
add_subdirectory(tests)