/*
 *  sync_stringbuf.hxx
 *  Copyright 2020-2022 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#pragma once

#include <mutex>
#include <sstream>

struct sync_stringbuf : public std::stringbuf {

    std::recursive_mutex mut;

  protected:
    std::streamsize xsgetn(char_type* s, std::streamsize n) override {
        std::lock_guard lk{mut};
        return std::stringbuf::xsgetn(s, n);
    }

    int_type underflow() override {
        std::lock_guard lk{mut};
        return std::stringbuf::underflow();
    }

    std::streamsize xsputn(const char_type* s, std::streamsize n) override {
        std::lock_guard lk{mut};
        return std::stringbuf::xsputn(s, n);
    }

    int_type overflow(int_type ch) override {
        std::lock_guard lk{mut};
        return std::stringbuf::overflow(ch);
    }

    int_type pbackfail(int_type c) override {
        std::lock_guard lk{mut};
        return std::stringbuf::pbackfail(c);
    }

    std::basic_stringbuf<char>::__streambuf_type* setbuf(char_type* s, std::streamsize n) override {
        std::lock_guard lk{mut};
        return std::stringbuf::setbuf(s, n);
    }

    pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                     std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override {
        std::lock_guard lk{mut};
        return std::stringbuf::seekoff(off, dir, which);
    }

    pos_type seekpos(pos_type sp,
                     std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override {
        std::lock_guard lk{mut};
        return std::stringbuf::seekpos(sp, which);
    }
};