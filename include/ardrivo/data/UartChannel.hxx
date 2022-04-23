#pragma once

#include <deque>
#include <mutex>
#include <span>
#include "forkmem/Atomic.hxx"
#include "forkmem/Mutex.hxx"
#include "forkmem/allocator.hxx"

class UartChannel {
  public:
    class Impl {
        frkm::IpcMutex mut;
        std::uint16_t max_buffered = 128;
        std::deque<char, frkm::polymorphic_allocator<char>> deque;

        friend UartChannel;
        Impl(frkm::polymorphic_allocator<std::byte> alloc) : deque{alloc} {}

      public:
        std::size_t max_size() noexcept { return max_buffered; }
        std::size_t size() noexcept { return deque.size(); }
        std::size_t write(std::span<const char> buf) noexcept {
            std::scoped_lock lk{mut};
            const auto count = std::min(std::clamp(max_buffered - deque.size(), std::size_t{0},
                                                   static_cast<std::size_t>(max_buffered)),
                                        buf.size());

            std::copy_n(buf.begin(), count, std::back_inserter(deque));
            return count;
        }
        std::size_t read(std::span<char> buf) noexcept {
            std::scoped_lock lk{mut};
            const auto count = std::min(deque.size(), buf.size());
            std::copy_n(deque.begin(), count, buf.begin());
            deque.erase(deque.begin(), deque.begin() + count);
            return count;
        }
        char front() noexcept {
            std::scoped_lock lk{mut};
            return deque.front();
        }
    };

  private:
    frkm::IpcAtomic<bool> begun;

    Impl rxch;
    Impl txch;

  public:
    void begin() noexcept { begun.store(true); }
    void end() noexcept { begun.store(false); }
    bool has_begun() noexcept { return begun.load(); }

    Impl& rx() { return rxch; }
    Impl& tx() { return txch; }

    UartChannel(frkm::polymorphic_allocator<std::byte> alloc) : rxch{alloc}, txch{alloc} {}
};