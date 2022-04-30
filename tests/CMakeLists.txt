
find_package (Catch2 QUIET)
if (NOT Catch2_FOUND)
  include (FetchContent)
  FetchContent_Declare (
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        devel
  )
  FetchContent_GetProperties (Catch2)
  if (NOT catch2_POPULATED)
    FetchContent_Populate (Catch2)
  endif ()
  set (CATCH_BUILD_STATIC_LIBRARY On CACHE INTERNAL "" FORCE)
  add_subdirectory ("${catch2_SOURCE_DIR}" "${catch2_BINARY_DIR}" EXCLUDE_FROM_ALL)
  list (APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
endif ()

include (CTest)
include (Catch)

add_executable (forkmem_tests
    basic.cxx
)


target_include_directories(forkmem_tests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries (forkmem_tests PUBLIC forkmem Catch2::Catch2WithMain)

catch_discover_tests (forkmem_tests)


add_executable(forkmem_tests_child)
target_sources(forkmem_tests_child PUBLIC child.cxx)
target_include_directories(forkmem_tests_child PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(forkmem_tests_child PUBLIC forkmem)

add_custom_command(TARGET forkmem_tests_child POST_BUILD COMMAND ${CMAKE_COMMAND} -E rename "$<TARGET_FILE:forkmem_tests_child>" "${CMAKE_CURRENT_BINARY_DIR}/forkmem_tests_child" )
target_compile_definitions(forkmem_tests PUBLIC TEST_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")



