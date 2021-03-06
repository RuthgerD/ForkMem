#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <string_view>
#include <thread>
#include <boost/process.hpp>
#include <boost/process/detail/child_decl.hpp>
#include <catch2/catch_test_macros.hpp>
#include "forkmem/Allocator.hxx"
#include "forkmem/Memory.hxx"
#include "forkmem/sync/Atomic.hxx"
#include "forkmem/sync/Mutex.hxx"

#include "Type.hxx"

#ifndef TEST_BINARY_DIR
#define TEST_BINARY_DIR "."
#endif

namespace bp = boost::process;

bp::child create_child(const std::string& name, void* userdata, const auto& handle) {
    return bp::child(
        bp::env["MAPVIEW_HANDLE"] = std::to_string(reinterpret_cast<std::uint64_t>(handle.handle)),
        bp::env["MAPVIEW_SIZE"] = std::to_string(handle.size),
        bp::env["MAPVIEW_PTR_BASE"] = std::to_string(reinterpret_cast<std::size_t>(handle.memory)),
        bp::env["USERDATA_PTR"] = std::to_string(reinterpret_cast<std::size_t>(userdata)), name,
        (bp::std_out & bp::std_err) > stdout);
}

const std::string sample_text =
    "hello world this is quite a long string, because compiler vendors are smart by inlining "
    "the first view bytes of the string into the object itself :)";

TEST_CASE("Basic Allocation", "[Memory]") {
    std::cout << "hello what the fuck\n";
    const auto size = 1024 * 100;
    auto memory = forkmem::Memory{size};
    auto pool = new (memory.data()) forkmem::MemoryPool{size - sizeof(forkmem::MemoryPool)};

    std::cout << "created pool?\n";

    auto allocator = forkmem::PoolAllocator<AllocatingType>{pool};

    auto* obj = allocator.new_object(allocator);

    REQUIRE(obj != nullptr);

    obj->str = sample_text;
    REQUIRE(obj->str.size() == sample_text.size());
    REQUIRE(std::strcmp(obj->str.data(), sample_text.data()) == 0);

    REQUIRE(!obj->flag.load());

    auto child = create_child(TEST_BINARY_DIR "/forkmem_tests_child", obj, memory.native_handle());
    REQUIRE(child.running());

    while (!obj->flag.load()) {
        if (!child.running()) {
            REQUIRE(child.exit_code() == 0);
            REQUIRE(false);
            break;
        }
        std::this_thread::yield();
    }

    if (child.running()) {
        child.terminate();
        child.exit_code();
    } else {
        REQUIRE(child.exit_code() == 0);
    }

    std::string out;
    for (auto c : obj->channel) {
        out.push_back(c);
    }

    REQUIRE(std::string_view{out} == std::string_view{obj->str});

    allocator.delete_object(obj);
}