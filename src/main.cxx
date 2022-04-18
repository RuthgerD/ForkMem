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
#include "ardrivo/data/SharedBoard.hxx"
#include "forkmem/unix/bridge.hxx"
#include "forkmem/unix/executable.hxx"
#include "ssmce/Sketch.hxx"
#include "ssmce/sync_stringbuf.hxx"

int main() {

    auto sketch = Sketch{
        stdfs::absolute("demo"),
        {Library{.type =
                     Library::Imported{
                         .name = "ardrivo",
                         .incdirs = {stdfs::absolute("include/ardrivo/lib")},
                         .location = stdfs::absolute("build/libardrivo.so"),
                         .implib = stdfs::absolute("build/libardrivo.so"),
                     }},
         {Library{
             .type =
                 Library::Source{
                     .name = "Smartcar_shield",
                     .depends = {"ardrivo"},
                     .uri = "https://github.com/platisd/smartcar_shield/archive/refs/tags/7.0.1.tar.gz",
                     .patch_uri =
                         "file://" + stdfs::absolute("share/patches/smartcar_shield/patch/").generic_string(),
                     .incdirs = {"src/"},
                     .sources = {"src/*.cpp", "src/*.hpp", "src/*.c", "src/*.h", "src/*.cxx", "src/*.hxx",
                                 "src/*.c++", "src/*.h++"},
                 }}}},
        stdfs::absolute("build")};

    auto sync_buf = sync_stringbuf{};
    auto log_read = std::jthread{[&](auto stop_token) {
        std::string out;
        while (std::getline(std::istream{&sync_buf}, out) || !stop_token.stop_requested()) {
            if (!out.empty())
                std::cout << out << "\n";
            std::this_thread::yield();
        }
    }};

    auto res = sketch.compile(std::ostream{&sync_buf});

    log_read.request_stop();
    log_read.join();

    if (!res) {
        std::cout << "Failed to compile sketch\n";
        return 1;
    }

    auto bridge = frkm::UserBridge<SharedBoard>{};
    auto& pins = bridge.user_data().pins;
    pins.emplace(0, 0);

    bridge.start(*sketch.executable());

    std::this_thread::sleep_for(std::chrono::milliseconds{300});

    bridge.stop();

    std::string buf;
    buf.resize(512);
    const auto count = bridge.user_data().serial0.tx().read({buf.data(), buf.size() - 1});
    buf.resize(count);

    std::cout << buf << "\n";
    std::cout << "pin value: " << pins.at(0).read() << "\n";
}