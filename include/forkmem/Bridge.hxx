#pragma once

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

    UserBridge() { data = frkm::new_object(get_allocator<T>(), get_allocator<std::byte>()); }

    T& user_data() { return *data; }

    void start(const Executable& exec) { return Bridge::start(exec, static_cast<void*>(data)); }

    ~UserBridge() { frkm::delete_object(get_allocator<T>(), data); }
};

} // namespace frkm