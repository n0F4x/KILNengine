module;

#include <algorithm>

export module kiln.gfx.vulkan.extension_inspection.extension_is_promoted_to_vulkan1x;

import vulkan_hpp;

import kiln.gfx.vulkan.extension_inspection.extensions_promoted_to_vulkan1x;
import kiln.util.StringLiteral;

namespace kiln::gfx::vulkan {

export [[nodiscard]]
constexpr auto extension_is_promoted_to_vulkan11(const util::StringLiteral extension_name)
    -> bool
{
    return std::ranges::contains(extensions_promoted_to_vulkan11(), extension_name);
}

export [[nodiscard]]
constexpr auto extension_is_promoted_to_vulkan12(const util::StringLiteral extension_name)
    -> bool
{
    return std::ranges::contains(extensions_promoted_to_vulkan12(), extension_name);
}

export [[nodiscard]]
constexpr auto extension_is_promoted_to_vulkan13(const util::StringLiteral extension_name)
    -> bool
{
    return std::ranges::contains(extensions_promoted_to_vulkan13(), extension_name);
}

export [[nodiscard]]
constexpr auto extension_is_promoted_to_vulkan14(const util::StringLiteral extension_name)
    -> bool
{
    return std::ranges::contains(extensions_promoted_to_vulkan14(), extension_name);
}

export [[nodiscard]]
constexpr auto extension_is_promoted_to_vulkan1x(const util::StringLiteral extension_name)
    -> bool
{
    return std::ranges::contains(extensions_promoted_to_vulkan1x(), extension_name);
}

}   // namespace kiln::gfx::vulkan
