
#ifndef FORKMEM_TESTS_TYPE_HXX
#define FORKMEM_TESTS_TYPE_HXX
#include "forkmem/Allocator.hxx"
#include "forkmem/sync/Atomic.hxx"
#include "forkmem/sync/StringBuf.hxx"

namespace forkmem::pmr {
using string = std::basic_string<char, std::char_traits<char>, forkmem::polymorphic_allocator<char>>;

}

struct AllocatingType {
    forkmem::pmr::string str;

    forkmem::pmr::syncstringbuf channel;

    forkmem::Atomic<bool> flag;

    AllocatingType(forkmem::polymorphic_allocator<char> alloc) : str{alloc}, channel{alloc} {}
};

#endif