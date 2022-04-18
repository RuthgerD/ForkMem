/*
 *  sketch_main.cxx
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
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "ardrivo/data/SharedBoard.hxx"

SharedBoard* get_user_data() {
    thread_local SharedBoard* user_data;
    if (user_data == nullptr) {
        const auto user_data_ptr_str = std::getenv("USERDATA_PTR");
        auto sus_pointer_num = std::stol(user_data_ptr_str);
        auto pointer = reinterpret_cast<void*>(sus_pointer_num);
        user_data = static_cast<SharedBoard*>(pointer);
    }
    return user_data;
}

void sketch_main(void (*setup)(), void (*loop)()) {
    std::cout << "sketch main? \n";

    (*setup)();

    for (int i = 0; i < 5; ++i) {
        (*loop)();
    }
}
