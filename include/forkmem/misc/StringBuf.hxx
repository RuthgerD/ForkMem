
#ifndef FORKMEM_STRINGBUF_HXX
#define FORKMEM_STRINGBUF_HXX

#include <mutex>
#include <sstream>
#include <streambuf>
#include <string>
#include "forkmem/Allocator.hxx"
#include "forkmem/sync/RecursiveMutex.hxx"

namespace forkmem {

template <class CharT, class Allocator>
class basic_syncstringbuf : public std::basic_stringbuf<CharT, std::char_traits<CharT>, Allocator> {
    RecursiveMutex mut;

    using int_type = typename std::char_traits<CharT>::int_type;
    using off_type = typename std::char_traits<CharT>::off_type;
    using char_type = typename std::char_traits<CharT>::char_type;
    using pos_type = typename std::char_traits<CharT>::pos_type;

    using Base = std::basic_stringbuf<CharT, std::char_traits<CharT>, Allocator>;

  public:
    basic_syncstringbuf(Allocator allocator)
        : std::basic_stringbuf<CharT, std::char_traits<CharT>, Allocator>{allocator} {}

  protected:
    std::streamsize xsgetn(CharT* s, std::streamsize n) override {
        std::lock_guard lk{mut};
        return Base::xsgetn(s, n);
    }

    int_type underflow() override {
        std::lock_guard lk{mut};
        return Base::underflow();
    }

    std::streamsize xsputn(const char_type* s, std::streamsize n) override {
        std::lock_guard lk{mut};
        return Base::xsputn(s, n);
    }

    int_type overflow(int_type ch) override {
        std::lock_guard lk{mut};
        return Base::overflow(ch);
    }

    int_type pbackfail(int_type c) override {
        std::lock_guard lk{mut};
        return Base::pbackfail(c);
    }

    std::basic_streambuf<CharT, std::char_traits<CharT>>* setbuf(char_type* s, std::streamsize n) override {
        std::lock_guard lk{mut};
        return Base::setbuf(s, n);
    }

    pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                     std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override {
        std::lock_guard lk{mut};
        return Base::seekoff(off, dir, which);
    }

    pos_type seekpos(pos_type sp,
                     std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override {
        std::lock_guard lk{mut};
        return Base::seekpos(sp, which);
    }
};

namespace pmr {
using syncstringbuf = basic_syncstringbuf<char, polymorphic_allocator<char>>;
}

} // namespace forkmem

#endif