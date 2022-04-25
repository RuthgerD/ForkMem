#include <cstring>
#include <type_traits>
#include <boost/atomic/detail/core_operations.hpp>
#include <boost/atomic/detail/wait_operations.hpp>
#include <boost/memory_order.hpp>
#include "forkmem/sync/Atomic.hxx"

namespace forkmem {

using atomic_ops = boost::atomics::detail::core_operations<sizeof(std::uint64_t), false, true>;
using native_storage_t = atomic_ops::storage_type;
using wait_ops = boost::atomics::detail::wait_operations<atomic_ops>;

static_assert(atomic_ops::is_always_lock_free);
static_assert(std::is_same_v<OpaqueAtomic::storage_t, atomic_ops::storage_type>);

OpaqueAtomic::storage_t OpaqueAtomic::load(std::memory_order order) const noexcept {

    return atomic_ops::load(storage, static_cast<boost::memory_order>(order));
}

void OpaqueAtomic::store(storage_t val, std::memory_order order) noexcept {
    atomic_ops::store(storage, val, static_cast<boost::memory_order>(order));
}

OpaqueAtomic::storage_t OpaqueAtomic::exchange(storage_t desired, std::memory_order order) noexcept {
    return atomic_ops::exchange(storage, desired, static_cast<boost::memory_order>(order));
}

bool OpaqueAtomic::compare_exchange_weak(storage_t& expected, storage_t desired, std::memory_order success,
                                         std::memory_order failure) noexcept {
    return atomic_ops::compare_exchange_weak(storage, expected, desired,
                                             static_cast<boost::memory_order>(success),
                                             static_cast<boost::memory_order>(failure));
}

bool OpaqueAtomic::compare_exchange_strong(storage_t& expected, storage_t desired, std::memory_order success,
                                           std::memory_order failure) noexcept {
    return atomic_ops::compare_exchange_strong(storage, expected, desired,
                                               static_cast<boost::memory_order>(success),
                                               static_cast<boost::memory_order>(failure));
}

void OpaqueAtomic::wait(storage_t old, std::memory_order order) const noexcept {
    wait_ops::wait(storage, old, static_cast<boost::memory_order>(order));
}

void OpaqueAtomic::notify_one() noexcept { wait_ops::notify_one(storage); }

void OpaqueAtomic::notify_all() noexcept { wait_ops::notify_all(storage); }

} // namespace forkmem