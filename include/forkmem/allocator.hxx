/*
 *  allocator.hxx
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

#include <utility>
#ifdef __has_include
#if __has_include(<memory_resource>)
#include <memory_resource>
namespace frkm {
template <class T> using polymorphic_allocator = std::pmr::polymorphic_allocator<T>;

using memory_resource = std::pmr::memory_resource;

} // namespace frkm
#elif __has_include(<experimental/memory_resource>)
#include <experimental/memory_resource>
namespace frkm {
template <class T> using polymorphic_allocator = std::experimental::pmr::polymorphic_allocator<T>;

using memory_resource = std::experimental::pmr::memory_resource;
} // namespace frkm
#else
#error "Missing <memory_resource>"
#endif
#endif

namespace frkm {
template <class T, class... Args> T* new_object(polymorphic_allocator<T> alloc, Args... args) {
    auto* p = alloc.allocate(1);
    try {
        alloc.construct(p, std::forward<Args>(args)...);
    } catch (...) {
        alloc.deallocate(p, 1);
        throw;
    }
    return p;
}

template <class T> void delete_object(polymorphic_allocator<T> alloc, T* p) {
    p->~T();
    alloc.deallocate(p, 1);
}

} // namespace frkm
