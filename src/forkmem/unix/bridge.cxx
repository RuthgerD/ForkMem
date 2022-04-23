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

#if defined(__unix__) || defined(__APPLE__)

#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory_resource>
#include <mutex>
#include <string>
#include <thread>
#include <dlfcn.h>
#include <unistd.h>
#include "forkmem/allocator.hxx"
#include "forkmem/unix/bridge.hxx"

namespace frkm {

struct Bridge::Control {
    int parent_pid;
    int child_pid;

    IpcMutex start;
};

void Bridge::start(const Executable& exec, void* user_data) {
    control = memory.get_allocator().new_object<Control>();

    std::scoped_lock start_lk{control->start};

    std::thread([&, control = control, user_data = user_data] {
        auto pid = fork();
        if (pid != 0) {
            control->child_pid = pid;
            control->parent_pid = getpid();
            return;
        }

        {
            auto string_ptr = std::to_string(reinterpret_cast<std::size_t>(user_data));
            setenv("USERDATA_PTR", string_ptr.c_str(), true);
        }

        auto dlhandle = dlopen(exec.library.c_str(), RTLD_LAZY);
        auto entry_func = reinterpret_cast<void (*)()>(dlsym(dlhandle, exec.entry.c_str()));
        std::cout << "waiting to enter library ";
        std::cout << &control->start << "\n";

        std::scoped_lock start_lk{control->start};

        std::cout << "waiting to enter library finished\n";

        if (!entry_func) {
            std::cout << "entry func doesn't exist :(\n";
            std::cout << "on: " << exec.library << "::" << exec.entry << "\n";
            perror("error: ");
            return;
        }
        (*entry_func)();
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

#endif