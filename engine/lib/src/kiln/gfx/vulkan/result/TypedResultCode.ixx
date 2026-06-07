module;

#include <type_traits>

export module kiln.gfx.vulkan.result.TypedResultCode;

import vulkan_hpp;

namespace kiln::gfx::vulkan {

export template <vk::Result value_T>
struct TypedResultCode {
    constexpr static std::integral_constant<vk::Result, value_T> value;
};

}   // namespace kiln::gfx::vulkan
