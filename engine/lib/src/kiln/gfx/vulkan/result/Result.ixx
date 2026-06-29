module;

#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include "kiln/util/contract_macros.hpp"

export module kiln.gfx.vulkan.result.Result;

import vulkan;

import kiln.gfx.vulkan.result.make_typed_result_code_variant;
import kiln.gfx.vulkan.result.result_category_from;
import kiln.gfx.vulkan.result.ResultCategory;
import kiln.gfx.vulkan.result.TypedResultCode;
import kiln.util.contracts;
import kiln.util.concepts.naked;
import kiln.util.type_traits.forward_like;

namespace kiln::gfx::vulkan {

[[nodiscard]]
constexpr auto ensure_success(const vk::Result result_code) -> vk::Result
{
    PRECOND(result_category_from(result_code) == ResultCategory::eSuccess);

    return result_code;
}

[[nodiscard]]
constexpr auto ensure_error(const vk::Result result_code) -> vk::Result
{
    PRECOND(result_category_from(result_code) != ResultCategory::eSuccess);

    return result_code;
}

export template <util::naked_c Value_T, vk::Result... expected_result_codes_T>
    requires((expected_result_codes_T == vk::Result::eSuccess) || ...)
class Result {
    [[nodiscard]]
    constexpr static auto ensure_expected(const vk::Result result_code) -> vk::Result
    {
        PRECOND(((result_code == expected_result_codes_T) || ...));
        return result_code;
    }

public:
    static_assert(!std::is_same_v<Value_T, vk::Result>);

    explicit Result(Value_T&& value, const vk::Result result_code)
        : m_value{ std::move(value) },
          m_result_code{ ensure_success(ensure_expected(result_code)) }
    {
    }

    explicit Result(const vk::Result result_code)
        : m_result_code{ ensure_error(ensure_expected(result_code)) }
    {
    }

    template <typename Self_T>
    [[nodiscard]]
    auto operator*(this Self_T&& self) -> util::forward_like_t<Value_T, Self_T>
    {
        return std::forward_like<Self_T>(*self.Result::m_value);
    }

    [[nodiscard]]
    auto result_code() const noexcept
        -> std::variant<TypedResultCode<expected_result_codes_T>...>
    {
        return make_typed_result_code_variant<expected_result_codes_T...>(m_result_code);
    }

private:
    std::optional<Value_T> m_value;
    vk::Result             m_result_code;
};

}   // namespace kiln::gfx::vulkan
