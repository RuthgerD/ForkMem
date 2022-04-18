/*
 *  Library.hxx
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

#include <string>
#include <variant>
#include <vector>

struct Library {
    struct Source {
        std::string name;
        std::vector<std::string> depends;
        std::string uri;
        std::string patch_uri;
        std::vector<std::string> incdirs;
        std::vector<std::string> sources;
    };

    struct Imported {
        std::string name;
        std::vector<std::string> incdirs;
        std::string location;
        std::string implib;
    };

    std::variant<Source, Imported> type;
};

std::string to_cmake_list(const std::vector<std::string>& vec);
std::string to_cmake(const Library::Source& source);
std::string to_cmake(const Library::Imported& imported);