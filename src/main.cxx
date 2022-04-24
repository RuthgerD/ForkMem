/*
 *  main.cxx
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
#include <mutex>
#include "forkmem/Bridge.hxx"
#include "ssmce/Sketch.hxx"
#include "ssmce/sync_stringbuf.hxx"
#include "TestData.hxx"

int main() {

    std::cout << "runtime!" << std::endl;

    auto bridge = frkm::UserBridge<TestData>{};
    std::cout << "bridge created!" << std::endl;

#if defined(__unix__)
    const auto execut =
        frkm::Bridge::Executable{.library = "./build/libforkmem-test-child.so", .entry = "entry"};
#elif defined(__APPLE__)
    const auto execut =
        frkm::Bridge::Executable{.library = "./build/libforkmem-test-child.dylib", .entry = "entry"};
#elif defined(_WIN32)
    const auto execut = frkm::Bridge::Executable{.path = "./build/forkmem-test-child.exe"};

#endif

    bridge.start(execut);
    std::cout << "bridge started!" << std::endl;

    auto& dat = bridge.user_data();
    std::cout << "got user data" << &dat << std::endl;

    while (true) {
        std::cout << "parent: triyng to lock " << std::endl;
        std::scoped_lock lk{dat.lk};
        if (dat.num % 2) {
            std::cout << "parent incremented" << std::endl;
            dat.num += 1;
        }
        if (dat.num == 1000)
            break;
    }

    bridge.stop();

    std::cout << "should be: " << dat.num << std::endl;
}