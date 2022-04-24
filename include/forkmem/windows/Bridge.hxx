#pragma once

#include <memory>
#include <string>
#include "forkmem/windows/IpcMemory.hxx"

namespace frkm {
class Bridge {
    IpcMemory memory{1024 * 100};
    struct Control;
    Control* control;

  public:
    template <class T> polymorphic_allocator<T> get_allocator() { return {memory.get_resource()}; }

    struct Executable {
        std::string path;
    };
    void start(const Executable& exec, void* user_data);

    bool tick();

    void suspend();
    void resume();
    void stop();

    ~Bridge();
};

} // namespace frkm
