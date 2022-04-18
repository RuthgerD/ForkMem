/*
 *  Sketch.hxx
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
#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <optional>
#include <ostream>
#include <sstream>
#include <thread>
#include <variant>
#include <vector>
#include "forkmem/unix/executable.hxx"
#include "ssmce/Library.hxx"
#include "ssmce/Uuid.hxx"

namespace stdfs = std::filesystem;
struct Sketch {
    stdfs::path path;
    stdfs::path build_path;

    struct CompiledInfo {
        stdfs::path build_path;
        frkm::Executable executable;

        bool is_valid();

        ~CompiledInfo() {
            std::error_code ec;
            stdfs::remove(build_path, ec);
        }
    };

    std::optional<CompiledInfo> compiled_info;

    std::string find_cmake_path();

    std::vector<Library> libraries;

  public:
    Sketch(stdfs::path path, std::vector<Library> libraries, stdfs::path build_path)
        : path{path}, libraries{libraries}, build_path{build_path} {}

    bool compile(std::basic_ostream<char> log);

    const frkm::Executable* executable() const {
        return compiled_info ? &compiled_info->executable : nullptr;
    }

    bool is_compiled() const { return executable(); }

    void reset() { compiled_info = {}; }
};
