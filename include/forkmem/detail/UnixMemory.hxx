

#ifndef FORKMEM_UNIXMEMORY_HXX
#define FORKMEM_UNIXMEMORY_HXX

#include <cstddef>
#include <cstdint>
namespace forkmem {

class UnixMemory {
  public:
    struct Handle {
        std::uint64_t handle;
        void* memory;
        std::size_t size;
    };

    UnixMemory(std::size_t size);

    static void make_reachable(const Handle& handle);

    unsigned char* data();

    const Handle& native_handle() const { return handle; }

    ~UnixMemory();

  private:
    Handle handle;
};
} // namespace forkmem

#endif