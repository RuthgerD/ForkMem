/*
 *  Bridge.hxx
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

#if defined(__unix__) || defined(__APPLE__)
#pragma once

#include "forkmem/unix/IpcMemory.hxx"

namespace frkm {
class Bridge {
    IpcMemory memory{1024 * 100};
    struct Control;
    Control* control;

  public:
    template <class T> polymorphic_allocator<T> get_allocator() { return {memory.get_resource()}; }

    struct Executable {
        std::string library;
        std::string entry;
    };
    void start(const Executable& exec, void* user_data);

    bool tick() { return true; }

    void suspend();
    void resume();
    void stop();

    ~Bridge();
};

}; // namespace frkm
#endif