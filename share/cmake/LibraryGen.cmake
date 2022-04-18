#
#  LibraryGen.cmake
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

function(resolve_source_path uri dest)
    if (uri MATCHES "^file://(.+)$")
        message(STATUS "[PLugin ${library_name}] Grabbing sources at ${uri}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_MATCH_1}" "${dest}"
        )
    elseif(uri MATCHES "^https?://(.+)$")
        message(STATUS "[PLugin ${library_name}] Downloading sources")
        set(ark_location "${root}/${library_name}.ark")
        file (DOWNLOAD "${CMAKE_MATCH_1}" "${ark_location}" SHOW_PROGRESS STATUS dlstatus)
        if (NOT dlstatus EQUAL "0")
            file(REMOVE "${ark_location}")
            message(FATAL_ERROR "[PLugin ${library_name}] Failed to download sources ${dlstatus}")
        endif()
        message(STATUS "[PLugin ${library_name}] Inflating sources")
        set(extract_dest "${dest}/.extract")
        file (ARCHIVE_EXTRACT INPUT "${ark_location}" DESTINATION "${extract_dest}")
        file (GLOB extracted_files "${extract_dest}/*")
        list (LENGTH extracted_files extracted_files_count)

        if (extracted_files_count EQUAL 0)
            message(FATAL_ERROR "[PLugin ${library_name}] No sources in download sources")
        else()
            message(STATUS "[PLugin ${library_name}] Unwrapping source ark")
            if(extracted_files_count EQUAL 1 AND IS_DIRECTORY "${extracted_files}")
                resolve_source_path("file://${extracted_files}" ${dest})
            else()
                resolve_source_path("file://${extract_dest}" ${dest})
            endif()
        endif()
        file(REMOVE "${ark_location}")
    else()
        message(FATAL_ERROR "[PLugin ${library_name}] Malformed uri ${uri}")
    endif()
endfunction(resolve_source_path)

function(make_static_library library_name source_root source_patch_root library_incdirs library_sources library_linklibs)
    set(root ${CMAKE_BINARY_DIR}/${library_name})
    file(MAKE_DIRECTORY ${root})

    resolve_source_path(${source_root} ${root})

    if(NOT ${source_patch_root} EQUAL "")
        resolve_source_path(${source_patch_root} ${root})
    endif()
    
    message(STATUS "[PLugin ${library_name}] Root directory is ${root}")


    list(TRANSFORM library_incdirs PREPEND ${root}/)
    list(TRANSFORM library_sources PREPEND ${root}/)

    file(GLOB_RECURSE actual_sources ${library_sources})

    add_library(${library_name} STATIC)
    target_include_directories(${library_name} PUBLIC ${library_incdirs})
    target_sources(${library_name} PUBLIC ${actual_sources})
    target_link_libraries(${library_name} PUBLIC ${library_linklibs})
endfunction(make_static_library)

function(make_imported_library library_name library_incdirs library_location library_implib)
    add_library(${library_name} SHARED IMPORTED)
    target_include_directories(${library_name} SYSTEM INTERFACE ${library_incdirs})
    set_property (TARGET ${library_name} PROPERTY IMPORTED_LOCATION ${library_location})
    set_property (TARGET ${library_name} PROPERTY IMPORTED_IMPLIB ${library_implib})
endfunction(make_imported_library)

