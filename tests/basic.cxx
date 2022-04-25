#include <cstdio>
#include <cstring>
#include <iostream>
#include <istream>
#include <string>
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

#if defined(__unix__) || defined(__APPLE__)
#include <dlfcn.h>
boost::process::child create_child(const std::string& name, void* userdata) {
    auto child_pid = 0;

    std::thread([&, name, userdata] {
        const auto pid = ::fork();

        if (pid != 0) {
            child_pid = pid;
            return;
        }

        auto handle = ::dlopen(name.c_str(), RTLD_LAZY);
        if (handle == nullptr) {
            return;
        }
        auto userdata_ptr = std::to_string(reinterpret_cast<std::size_t>(userdata));
        ::setenv("USERDATA_PTR", userdata_ptr.c_str(), true);
        auto entry_func = reinterpret_cast<void (*)()>(::dlsym(handle, "main"));
        if (entry_func == nullptr) {
            return;
        }
        (*entry_func)();
    }).join();

    REQUIRE(child_pid != 0);

    return boost::process::child{child_pid};
}
#elif defined(_WIN32)
void create_child(const std::string& name, void* userdata, const Win32Memory::Handle handle& handle) {
    control->proc =
        bp::child(bp::env["MAPVIEW_HANDLE"] = std::to_string(reinterpret_cast<std::size_t>(handle.handle)),
                  bp::env["MAPVIEW_SIZE"] = std::to_string(handle.size),
                  bp::env["MAPVIEW_PTR_BASE"] = std::to_string(reinterpret_cast<std::size_t>(handle.memory)),
                  bp::env["USERDATA_PTR"] = std::to_string(reinterpret_cast<std::size_t>(userdata)), name,
                  (bp::std_out & bp::std_err) > stdout);
}
#endif

const std::string sample_text =
    "hello world this is quite a long string, because compiler vendors are smart by inlining "
    "the first view bytes of the string into the object itself :)";

TEST_CASE("Basic Allocation", "[Memory]") {
    auto memory = forkmem::Memory{1024 * 100};

    auto allocator = forkmem::polymorphic_allocator<AllocatingType>(memory.get_resource());

    auto* obj = forkmem::new_object(allocator, memory.get_resource());

    REQUIRE(obj != nullptr);

    obj->str = sample_text;
    REQUIRE(obj->str.size() == sample_text.size());
    REQUIRE(std::strcmp(obj->str.data(), sample_text.data()) == 0);

    REQUIRE(!obj->flag.load());

    auto child = create_child(TEST_BINARY_DIR "/forkmem_tests_child", obj);
    REQUIRE(child.running());

    while (!obj->flag.load()) {
        if (!child.running()) {
            REQUIRE(false);
            break;
        }
        std::this_thread::yield();
    }

    if (child.running()) {
        child.terminate();
        child.exit_code();
    }

    auto read = std::istream{&obj->channel};
    auto out = std::string{};
    std::getline(read, out);

    REQUIRE(out == sample_text);

    forkmem::delete_object(allocator, obj);
}