
#ifndef FORKMEM_ATOMIC_HXX
#define FORKMEM_ATOMIC_HXX

#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>

namespace forkmem {

template <class To, class From> To bit_cast(const From& src) noexcept {
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

class OpaqueAtomic {
  public:
    using storage_t = std::uint64_t;
    [[nodiscard]] storage_t load(std::memory_order order = std::memory_order_seq_cst) const noexcept;

    void store(storage_t val, std::memory_order order = std::memory_order_seq_cst) noexcept;

    [[nodiscard]] storage_t exchange(storage_t desired,
                                     std::memory_order order = std::memory_order_seq_cst) noexcept;

    [[nodiscard]] bool compare_exchange_weak(storage_t& expected, storage_t desired,
                                             std::memory_order success, std::memory_order failure) noexcept;

    [[nodiscard]] bool compare_exchange_strong(storage_t& expected, storage_t desired,
                                               std::memory_order success, std::memory_order failure) noexcept;

    void wait(storage_t old, std::memory_order order = std::memory_order_seq_cst) const noexcept;

    void notify_one() noexcept;
    void notify_all() noexcept;

  private:
    volatile storage_t storage = {0};
};

template <class T> class Atomic {
    OpaqueAtomic opaque;

    OpaqueAtomic::storage_t opaque_cast(T val) {
        OpaqueAtomic::storage_t tmp = {0};
        memcpy(&tmp, &val, sizeof(val));
        return tmp;
    }

  public:
    [[nodiscard]] T load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
        return forkmem::bit_cast<T>(opaque.load());
    }

    void store(T val, std::memory_order order = std::memory_order_seq_cst) noexcept {
        opaque.store(opaque_cast(val));
    }

    [[nodiscard]] T exchange(T desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
        return forkmem::bit_cast<T>(opaque.exchange(opaque_cast(desired), order));
    }

    [[nodiscard]] bool compare_exchange_weak(T& expected, T desired, std::memory_order success,
                                             std::memory_order failure) noexcept {
        auto tmp = opaque_cast(expected);
        const auto ret = opaque.compare_exchange_weak(tmp, opaque_cast(desired), success, failure);
        if (!ret)
            expected = forkmem::bit_cast<T>(tmp);
        return ret;
    }

    [[nodiscard]] bool compare_exchange_strong(T& expected, T desired, std::memory_order success,
                                               std::memory_order failure) noexcept {
        auto tmp = opaque_cast(expected);
        const auto ret = opaque.compare_exchange_strong(tmp, opaque_cast(desired), success, failure);
        if (!ret)
            expected = forkmem::bit_cast<T>(tmp);
        return ret;
    }

    void wait(T old, std::memory_order order = std::memory_order_seq_cst) const noexcept {
        opaque.wait(opaque_cast(old), order);
    }

    void notify_one() noexcept { opaque.notify_one(); }

    void notify_all() noexcept { opaque.notify_all(); }
};
} // namespace forkmem

#endif