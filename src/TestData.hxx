#pragma once

#include <cstdint>
#include "forkmem/Atomic.hxx"
#include "forkmem/Mutex.hxx"
struct TestData {

    frkm::IpcMutex lk;
    std::uint32_t num;

    // frkm::IpcAtomic<std::uint32_t> num;

    TestData(auto) {}
};
