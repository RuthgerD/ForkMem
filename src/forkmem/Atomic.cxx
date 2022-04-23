#include <atomic>
#include <cstdint>
#include <boost/atomic/atomic.hpp>
#include <boost/atomic/detail/core_operations.hpp>
#include <boost/atomic/detail/wait_operations.hpp>
#include <boost/memory_order.hpp>
#include "forkmem/Atomic.hxx"

namespace frkm {
using ops = boost::atomics::detail::core_operations<sizeof(std::uint64_t), false, true>;
using wait_ops = boost::atomics::detail::wait_operations<ops>;

AtomicOps::storage_t AtomicOps::load(const volatile storage_t& storage, std::memory_order order) noexcept {
    return std::bit_cast<storage_t>(ops::load(reinterpret_cast<ops::storage_type const volatile&>(storage),
                                              static_cast<boost::memory_order>(order)));
}

void AtomicOps::store(volatile storage_t& storage, storage_t val, std::memory_order order) noexcept {
    ops::store(reinterpret_cast<ops::storage_type volatile&>(storage), std::bit_cast<ops::storage_type>(val),
               static_cast<boost::memory_order>(order));
}

AtomicOps::storage_t AtomicOps::exchange(volatile storage_t& storage, storage_t desired,
                                         std::memory_order order) noexcept {
    return std::bit_cast<storage_t>(ops::exchange(reinterpret_cast<ops::storage_type volatile&>(storage),
                                                  std::bit_cast<ops::storage_type>(desired),
                                                  static_cast<boost::memory_order>(order)));
}

void AtomicOps::wait(const volatile storage_t& storage, storage_t old, std::memory_order order) noexcept {
    wait_ops::wait(reinterpret_cast<const ops::storage_type volatile&>(storage),
                   std::bit_cast<ops::storage_type>(old), static_cast<boost::memory_order>(order));
}

void AtomicOps::notify_one(volatile storage_t& storage) noexcept {
    wait_ops::notify_one(reinterpret_cast<ops::storage_type volatile&>(storage));
}

void AtomicOps::notify_all(volatile storage_t& storage) noexcept {
    wait_ops::notify_all(reinterpret_cast<ops::storage_type volatile&>(storage));
}
} // namespace frkm