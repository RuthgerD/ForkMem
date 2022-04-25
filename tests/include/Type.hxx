
#ifndef FORKMEM_TESTS_TYPE_HXX
#define FORKMEM_TESTS_TYPE_HXX
#include <deque>
#include <iostream>
#include "forkmem/Allocator.hxx"
#include "forkmem/sync/Atomic.hxx"

namespace forkmem::pmr {
using string = std::basic_string<char, std::char_traits<char>, forkmem::polymorphic_allocator<char>>;
template <class T> using deque = std::deque<T, forkmem::polymorphic_allocator<T>>;
} // namespace forkmem::pmr

struct AllocatingType {
    forkmem::pmr::string str;

    forkmem::pmr::deque<char> channel;

    forkmem::Atomic<bool> flag;

    AllocatingType(forkmem::polymorphic_allocator<char> alloc) : str{alloc}, channel{alloc} {}
};

#endif