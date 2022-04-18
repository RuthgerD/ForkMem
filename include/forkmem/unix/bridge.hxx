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

#pragma once

#include <functional>
#include "forkmem/allocator.hxx"
#include "forkmem/unix/executable.hxx"

namespace frkm {
class Bridge {
    struct Memory {
        struct Resource {
            monotonic_buffer_resource buf;
            synchronized_pool_resource sync_buf;

            Resource(char* buffer, std::size_t size) : buf{buffer, size}, sync_buf(&buf) {}
        };

        void* mmap_buffer;
        std::size_t mmap_size;

        Resource* resource;

      public:
        Memory(std::size_t size);

        polymorphic_allocator<std::byte> get_allocator();

        ~Memory();
    };

    Memory memory{1024 * 100};

    struct Control;

    Control* control;

  protected:
    polymorphic_allocator<std::byte> get_allocator() { return memory.get_allocator(); }

  public:
    void start(const Executable& exec, void* user_data);

    void suspend();
    void resume();

    void stop();

    ~Bridge();
};

template <class T> class UserBridge : public Bridge {
    T* data;

  public:
    UserBridge() { data = get_allocator().template new_object<T>(get_allocator()); }

    T& user_data() { return *data; }

    void start(const Executable& exec) { return Bridge::start(exec, static_cast<void*>(data)); }

    ~UserBridge() { get_allocator().delete_object(data); }
};

}; // namespace frkm