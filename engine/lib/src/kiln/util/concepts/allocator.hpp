#pragma once

#include <concepts>
#include <cstddef>

#include "kiln/util/concepts/naked.hpp"

namespace kiln::util {

template <typename T>
concept allocator_c = naked_c<T> && requires {
    typename T::value_type;
} && requires(T allocator, typename T::value_type* pointer, std::size_t n) {
    { allocator.allocate(n) } -> std::same_as<typename T::value_type*>;
    { allocator.deallocate(pointer, n) };
};

}   // namespace kiln::util
