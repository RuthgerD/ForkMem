/*
 *  Bridge.cxx
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

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory_resource>
#include <mutex>
#include <string>
#include <thread>
#include <boost/atomic.hpp>
#include <boost/atomic/atomic_flag.hpp>
#include <boost/container/container_fwd.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "forkmem/allocator.hxx"
#include "forkmem/unix/bridge.hxx"
#include "forkmem/unix/executable.hxx"

namespace frkm {

Bridge::Memory::Memory(std::size_t size) {
    mmap_buffer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    mmap_size = size;

    auto usable_buf = reinterpret_cast<char*>(mmap_buffer);
    resource = new (mmap_buffer) Resource{usable_buf + sizeof(Resource), mmap_size - sizeof(Resource)};
}

polymorphic_allocator<std::byte> Bridge::Memory::get_allocator() { return &resource->sync_buf; }

Bridge::Memory::~Memory() {
    resource->~Resource();
    munmap(mmap_buffer, mmap_size);
}

// void Bridge::start(const Executable& exec) { return start(exec, nullptr); }
using namespace boost::interprocess;

struct Bridge::Control {
    int parent_pid;
    int child_pid;

    interprocess_mutex start;
};

void Bridge::start(const Executable& exec, void* user_data) {
    control = memory.get_allocator().new_object<Control>();

    std::scoped_lock start_lk{control->start};

    std::thread([&, control = control, user_data = user_data] {
        // TODO: use clone3 instead cuz nice maybe?
        auto pid = fork();
        if (pid != 0) {
            control->child_pid = pid;
            control->parent_pid = getpid();
            return;
        }

        {
            // TODO: Determine pointer size, f*ck pointer provenance
            auto string_ptr = std::to_string(reinterpret_cast<uint64_t>(user_data));
            setenv("USERDATA_PTR", string_ptr.c_str(), true);
        }

        auto dlhandle = dlopen(exec.library.c_str(), RTLD_LAZY);
        auto entry_func = reinterpret_cast<void (*)()>(dlsym(dlhandle, exec.entry.c_str()));
        // TODO: sync wait before proceding
        std::cout << "waiting to enter library ";
        std::cout << &control->start << "\n";

        std::scoped_lock start_lk{control->start};

        std::cout << "waiting to enter library finished\n";

        if (entry_func)
            (*entry_func)();
        else {
            std::cout << "entry func doesn't exist :(\n";
            std::cout << "on: " << exec.library << "::" << exec.entry << "\n";
            perror("error: ");
        }
    }).join();
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    std::cout << "setting start flag\n";
}

void Bridge::suspend() { kill(control->child_pid, SIGSTOP); }
void Bridge::resume() { kill(control->child_pid, SIGCONT); }

void Bridge::stop() { kill(control->child_pid, SIGKILL); }

Bridge::~Bridge() {
    stop();
    get_allocator().delete_object(control);
}

} // namespace frkm