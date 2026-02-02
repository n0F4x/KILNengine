module;

#include <concepts>
#include <cstddef>

export module kiln.util.concepts.allocator;

import kiln.util.concepts.naked;

namespace kiln::util {

export template <typename T>
concept allocator_c = naked_c<T> && requires {
    typename T::value_type;
} && requires(T allocator, typename T::value_type* pointer, std::size_t n) {
    { allocator.allocate(n) } -> std::same_as<typename T::value_type*>;
    { allocator.deallocate(pointer, n) };
};

}   // namespace kiln::util
