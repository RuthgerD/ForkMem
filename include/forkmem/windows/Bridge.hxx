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
    polymorphic_allocator<std::byte> get_allocator() { return memory.get_allocator(); }

    struct Executable {
        std::string path;
    };
    void start(const Executable& exec, void* user_data);

    void suspend();
    void resume();
    void stop();

    ~Bridge();
};

} // namespace frkm
