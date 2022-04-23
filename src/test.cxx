

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include "forkmem/child/Child.hxx"
#include "TestData.hxx"

TestData* get_user_data() {
    thread_local TestData* user_data;
    if (user_data == nullptr) {
        user_data = static_cast<TestData*>(get_data());
    }
    return user_data;
}

extern "C" void entry() {
    std::cout << "Hello world!\n";

    auto& dat = *get_user_data();
    while (true) {
        std::scoped_lock lk{dat.lk};
        if (!(dat.num % 2)) {
            dat.num += 1;
            std::cout << "child incremented, sleeping at: " << dat.num << "\n";
        }

        if (dat.num == 999)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}