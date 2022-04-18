/*
 *  HardwareSerial.cxx
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

#include <iostream>
#include "ardrivo/data/SharedBoard.hxx"
#include "ardrivo/lib/HardwareSerial.h"

SharedBoard* get_user_data();

UartChannel& channel() { return get_user_data()->serial0; }

void HardwareSerial::begin(unsigned long, std::uint8_t) { channel().begin(); }
void HardwareSerial::end() { channel().end(); }

int HardwareSerial::available() { return static_cast<int>(channel().rx().size()); }

int HardwareSerial::availableForWrite() {
    auto& tx_buf = channel().tx();
    return static_cast<int>(tx_buf.max_size() - tx_buf.size());
}

size_t HardwareSerial::write(uint8_t c) {
    return channel().tx().write({reinterpret_cast<const char*>(&c), 1});
}

size_t HardwareSerial::write(const uint8_t* buf, std::size_t n) {
    return channel().tx().write({reinterpret_cast<const char*>(buf), n});
}

int HardwareSerial::read() {
    char ret;
    return channel().rx().read({&ret, 1}) ? ret : -1;
}

int HardwareSerial::peek() {
    if (channel().rx().size() < 1)
        return -1;
    return channel().rx().front();
}

HardwareSerial Serial;