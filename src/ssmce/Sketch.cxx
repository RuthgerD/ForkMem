/*
 *  Sketch.cxx
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

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <variant>
#include <boost/process.hpp>
#include "cmake/CMakeLists.hxx"
#include "cmake/ConfigureSketch.hxx"
#include "cmake/LibraryGen.hxx"
#include "cmake/Preprocessing.hxx"
#include "cmake/ProbeCompilerIncdirs.hxx"
#include "snippets/SketchMain.hxx"

#include "ssmce/Sketch.hxx"
#include "ssmce/Uuid.hxx"

namespace bp = boost::process;

std::string Sketch::find_cmake_path() { return "/usr/bin/cmake"; }

bool Sketch::CompiledInfo::is_valid() { return stdfs::exists(build_path); }

bool Sketch::compile(std::basic_ostream<char> log) {
    if (!stdfs::exists(path))
        return false;

    if (!compiled_info || !compiled_info->is_valid()) {
        auto ret = CompiledInfo{};
        auto uuid = Uuid::generate();
        ret.build_path = build_path / ".tmp" / uuid.to_hex();
        ret.executable = {.library = ret.build_path / "build" / "Sketch", .entry = "sketch_entry"};

        compiled_info = std::move(ret);

        auto make_dirs = [](const auto& p) {
            if (std::error_code ec; !stdfs::create_directories(p, ec) || ec)
                return false;
            return true;
        };

        make_dirs(compiled_info->build_path);
        const auto modules = compiled_info->build_path / "CMake" / "Modules";
        make_dirs(modules);

        auto exp_file = [](const auto& path, auto out, auto size) {
            auto cm = std::ofstream{path, std::ios::binary};
            cm.write(out, size);
        };

        exp_file(compiled_info->build_path / "CMakeLists.txt", CMakeLists, sizeof(CMakeLists) - 1);
        exp_file(compiled_info->build_path / "sketch_main.cxx", SketchMain, sizeof(SketchMain) - 1);

        exp_file(modules / "Preprocessing.cmake", Preprocessing, sizeof(Preprocessing) - 1);
        exp_file(modules / "ConfigureSketch.cmake", ConfigureSketch, sizeof(ConfigureSketch) - 1);
        exp_file(modules / "LibraryGen.cmake", LibraryGen, sizeof(LibraryGen) - 1);
        exp_file(modules / "ProbeCompilerIncdirs.cmake", ProbeCompilerIncdirs,
                 sizeof(ProbeCompilerIncdirs) - 1);

        auto cm = std::ofstream{modules / "Libraries.cmake", std::ios::binary};
        cm << "include(LibraryGen)\n";
        for (const auto& library : libraries) {
            cm << std::visit([](const auto& type) { return to_cmake(type); }, library.type) << "\n";
        }
    }

    const auto cmake_path = find_cmake_path();

    auto cmake_config_stream = bp::ipstream{};

    auto build_path_str = compiled_info->build_path.generic_string();
    auto cmake_config = bp::child(cmake_path, "-DSKETCH_BUILD_PATH=" + build_path_str,
                                  "-DSKETCH_PATH=" + stdfs::absolute(path).generic_string(), "-P",
                                  build_path_str + "/CMake/Modules/ConfigureSketch.cmake",
                                  (bp::std_out & bp::std_err) > cmake_config_stream);

    std::string line;
    while (std::getline(cmake_config_stream, line)) {
        log << line << std::endl;
    }

    cmake_config.join();

    // TODO: detect failure xD

    return true;
}
