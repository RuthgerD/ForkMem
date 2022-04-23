#pragma once

#include <array>
#include <atomic>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace frkm {

struct AtomicOps {
    using storage_t = std::array<unsigned char, sizeof(std::uint64_t)>;

    [[nodiscard]] static storage_t load(const volatile storage_t& storage,
                                        std::memory_order order = std::memory_order_seq_cst) noexcept;

    static void store(volatile storage_t& storage, storage_t val,
                      std::memory_order order = std::memory_order_seq_cst) noexcept;

    [[nodiscard]] static storage_t exchange(volatile storage_t& storage, storage_t desired,
                                            std::memory_order order = std::memory_order_seq_cst) noexcept;

    static void wait(const volatile storage_t&, storage_t old,
                     std::memory_order order = std::memory_order_seq_cst) noexcept;

    static void notify_one(volatile storage_t&) noexcept;
    static void notify_all(volatile storage_t&) noexcept;
};

template <class T>
requires(std::has_single_bit(alignof(T)) && alignof(T) <= alignof(std::uint64_t) &&
         std::is_trivial_v<T>) struct IpcAtomic {
    using storage_t = AtomicOps::storage_t;

    alignas(alignof(T)) volatile storage_t storage{};

    static inline T from_opaque(storage_t opaque) {
        T ret;
        memcpy(&ret, opaque.data(), sizeof(T));
        return ret;
    }

    static inline storage_t to_opaque(T t) {
        storage_t ret;
        memcpy(ret.data(), &t, sizeof(T));
        return ret;
    }

    [[nodiscard]] T load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
        return from_opaque(AtomicOps::load(storage, order));
    }

    void store(T val, std::memory_order order = std::memory_order_seq_cst) noexcept {
        AtomicOps::store(storage, to_opaque(val), order);
    }

    T exchange(T desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
        return from_opaque(AtomicOps::exchange(storage, to_opaque(desired), order));
    }

    void wait(T old, std::memory_order order = std::memory_order::seq_cst) const noexcept {
        AtomicOps::wait(storage, to_opaque(old), order);
    }

    void notify_one() noexcept { AtomicOps::notify_one(storage); }
    void notify_all() noexcept { AtomicOps::notify_all(storage); }
};

} // namespace frkm