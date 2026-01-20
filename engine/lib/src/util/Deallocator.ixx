module;

#include <concepts>
#include <cstdint>

export module kiln.util.Deallocator;

import kiln.util.concepts.allocator;

namespace kiln::util {

export template <typename Allocator_T>
    requires(allocator_c<Allocator_T>)
class Deallocator {
public:
    template <typename UAllocator_T>
        requires std::constructible_from<Allocator_T, UAllocator_T&&>
    constexpr explicit Deallocator(UAllocator_T&& allocator)
        : m_allocator{ std::forward<UAllocator_T>(allocator) }
    {
    }

    template <typename T>
    constexpr auto operator()(T* pointer, std::size_t n = 1) -> void
    {
        m_allocator.deallocate(pointer, n);
    }

private:
    Allocator_T m_allocator;
};

}   // namespace kiln::util
