/*
 *  Arduino.cxx
 *  Copyright 2021-2022 ItJustWorksTM
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

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include "ardrivo/data/SharedBoard.hxx"
#include "ardrivo/lib/Arduino.h"

SharedBoard* get_user_data();

Pin* get_pin(std::size_t id) {
    auto& pins = get_user_data()->pins;
    if (pins.contains(id))
        return &pins.at(id);
    return nullptr;
}

void pinMode(int pin, bool mode) {
    auto error = [=](const char* msg) {
        std::cerr << "ERROR: pinMode(" << pin << ", " << (mode ? "OUTPUT" : "INPUT") << "): " << msg
                  << std::endl;
    };

    auto vpin = get_pin(pin);
    if (!vpin)
        return error("Pin does not exist");

    vpin->set_direction(static_cast<Pin::DataDirection>(+mode));
}

int digitalRead(int pin) {
    auto error = [=](const char* msg) {
        return std::cerr << "ERROR: digitalRead(" << pin << "): " << msg << std::endl, 0;
    };

    auto vpin = get_pin(pin);
    if (!vpin)
        return error("Pin does not exist");
    if (!vpin->can_read_digital())
        return error("Pin has no digital driver capable of reading");
    if (vpin->get_direction() != Pin::DataDirection::In)
        return error("Pin is in output mode");

    return vpin->read();
}

void digitalWrite(int pin, bool value) {
    auto error = [=](const char* msg) {
        std::cerr << "ERROR: digitalWrite(" << pin << ", " << (value ? "HIGH" : "LOW") << "): " << msg
                  << std::endl;
    };

    auto vpin = get_pin(pin);
    if (!vpin)
        return error("Pin does not exist");
    if (!vpin->can_write_digital())
        return error("Pin has no digital driver capable of reading");
    if (vpin->get_direction() != Pin::DataDirection::Out)
        return error("Pin is in input mode");

    vpin->write(value);
}

int analogRead(int pin) {
    auto error = [=](const char* msg) {
        return std::cerr << "ERROR: analogRead(" << pin << "): " << msg << std::endl, 0;
    };
    auto vpin = get_pin(pin);
    if (!vpin)
        return error("Pin does not exist");
    if (!vpin->can_read_analog())
        return error("Pin has no analog driver capable of reading");
    if (vpin->get_direction() != Pin::DataDirection::In)
        return error("Pin is in output mode");
    return vpin->read();
}

void analogWrite(int pin, byte value) {
    auto error = [=](const char* msg) {
        std::cerr << "ERROR: analogWrite(" << pin << ", " << static_cast<int>(value) << "): " << msg
                  << std::endl;
    };
    auto vpin = get_pin(pin);
    if (!vpin)
        return error("Pin does not exist");
    if (!vpin->can_write_analog())
        return error("Pin has no analog driver capable of reading");
    if (vpin->get_direction() != Pin::DataDirection::Out)
        return error("Pin is in input mode");

    vpin->write(value);
}

void delay(unsigned long long ms) { std::this_thread::sleep_for(std::chrono::milliseconds{ms}); }

void delayMicroseconds(unsigned long long us) { std::this_thread::sleep_for(std::chrono::microseconds{us}); }

static const auto start_time = std::chrono::steady_clock::now();

unsigned long micros() {
    const auto current_time = std::chrono::steady_clock::now();
    return static_cast<unsigned long>(
        std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time).count());
}

unsigned long millis() {
    const auto current_time = std::chrono::steady_clock::now();
    return static_cast<unsigned long>(
        std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count());
}