/*
 *  Library.cxx
 *  Copyright 2020-2022 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include <iomanip>
#include <sstream>
#include "ssmce/Library.hxx"

std::string to_cmake_list(const std::vector<std::string>& vec) {
    auto acc = std::string{};
    for (const auto& p : vec)
        acc += p + ";";
    if (!acc.empty())
        acc.pop_back();
    return acc;
}

std::string to_cmake(const Library::Source& source) {
    using std::quoted;
    std::stringstream out;
    out << "make_static_library(" << quoted(source.name) << " " << quoted(source.uri) << " "
        << quoted(source.patch_uri) << " " << quoted(to_cmake_list(source.incdirs)) << " "
        << quoted(to_cmake_list(source.sources)) << " " << quoted(to_cmake_list(source.depends)) << ")\n";
    out << "target_link_libraries(Sketch PRIVATE " << source.name << ")";
    return out.str();
}

std::string to_cmake(const Library::Imported& imported) {
    using std::quoted;
    std::stringstream out;
    out << "make_imported_library(" << quoted(imported.name) << " " << quoted(to_cmake_list(imported.incdirs))
        << " " << quoted(imported.location) << " " << quoted(imported.implib) << ")\n";
    out << "target_link_libraries(Sketch PRIVATE " << imported.name << ")";
    return out.str();
}