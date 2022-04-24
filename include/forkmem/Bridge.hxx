#pragma once

#include <iostream>
#if defined(__unix__) || defined(__APPLE__)
#include "forkmem/unix/bridge.hxx"
#elif defined(_WIN32)
#include "forkmem/windows/Bridge.hxx"
#endif

namespace frkm {

template <class T> class UserBridge : public Bridge {
    T* data;

  public:
    using Bridge::Executable;

    UserBridge() {
        std::cout << "User bridge CTOR" << std::endl;
        auto talloc = get_allocator<T>();
        auto balloc = get_allocator<std::byte>();
        data = frkm::new_object(talloc, balloc);
        std::cout << "User bridge CTOR made data: " << data << std::endl;
    }

    T& user_data() { return *data; }

    void start(const Executable& exec) { return Bridge::start(exec, static_cast<void*>(data)); }

    ~UserBridge() {
        auto talloc = get_allocator<T>();
        frkm::delete_object(talloc, data);
    }
};

} // namespace frkm