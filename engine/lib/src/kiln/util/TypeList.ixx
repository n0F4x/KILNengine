module;

#include <cstddef>

export module kiln.util.TypeList;

namespace kiln::util {

export template <typename... Ts>
struct TypeList {
    [[nodiscard]]
    consteval static auto size() noexcept -> std::size_t
    {
        return sizeof...(Ts);
    }
};

}   // namespace kiln::util
