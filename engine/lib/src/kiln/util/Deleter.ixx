module;

#include <functional>
#include <memory_resource>

#include "kiln/util/lifetimebound.hpp"

export module kiln.util.Deleter;

namespace kiln::util {

export class Deleter {
public:
    template <typename T>
    constexpr explicit Deleter([[kiln_lifetimebound]]
                               std::pmr::polymorphic_allocator<T>& allocator)
        : m_allocator{ allocator }
    {
    }

    template <typename T>
    constexpr auto operator()(T* pointer) -> void
    {
        m_allocator.get().delete_object(pointer);
    }

private:
    std::reference_wrapper<std::pmr::polymorphic_allocator<>> m_allocator;
};

}   // namespace kiln::util
