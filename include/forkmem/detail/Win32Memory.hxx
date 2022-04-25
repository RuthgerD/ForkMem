
#ifndef FORKMEM_WIN32MEMORY_HXX
#define FORKMEM_WIN32MEMORY_HXX

#include <cstddef>
namespace forkmem {

class Win32Memory {
  public:
    struct Handle {
        void* handle;
        void* memory;
        std::size_t size;
    };

    Win32Memory(std::size_t size);
    Win32Memory(void* handle, void* memory, std::size_t size);

    unsigned char* data();

    const Handle& native_handle() const { return handle; }

    ~Win32Memory();

  private:
    Handle handle;
};
} // namespace forkmem

#endif