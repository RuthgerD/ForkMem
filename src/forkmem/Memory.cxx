#include <cstddef>
#include "forkmem/Memory.hxx"
#include "forkmem/PoolResource.hxx"

namespace forkmem {

Memory::Memory(std::size_t size) : memory{size} {
    resource =
        new (memory.data()) PoolResource(memory.data() + sizeof(PoolResource), size - sizeof(PoolResource));
}

memory_resource* Memory::get_resource() { return resource; }

Memory::~Memory() { dynamic_cast<PoolResource*>(resource)->~PoolResource(); }

} // namespace forkmem