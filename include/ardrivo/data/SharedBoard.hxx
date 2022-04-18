#pragma once

#include <algorithm>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>
#include <span>
#include <unordered_map>
#include <utility>
#include "ardrivo/data/Pin.hxx"
#include "ardrivo/data/UartChannel.hxx"
#include "forkmem/allocator.hxx"

template <class K, class V>
using pmr_unordered_map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>,
                                             frkm::polymorphic_allocator<std::pair<const K, V>>>;

struct SharedBoard {
    UartChannel serial0;

    pmr_unordered_map<std::size_t, Pin> pins;

    SharedBoard(frkm::polymorphic_allocator<std::byte> alloc) : serial0{alloc}, pins{alloc} {}
};