#pragma once

#include <cstddef>

namespace kiln::util {

template <typename... Ts>
struct TypeList {
    [[nodiscard]]
    consteval static auto size() noexcept -> std::size_t
    {
        return sizeof...(Ts);
    }
};

}   // namespace kiln::util
