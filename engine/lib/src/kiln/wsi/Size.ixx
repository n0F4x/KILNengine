module;

#include <concepts>
#include <cstdint>

export module kiln.wsi.Size;

import vulkan_hpp;

namespace kiln::wsi {

export struct Size2i {
    static_assert(std::same_as<int, int32_t>);

    int32_t width;
    int32_t height;
};

export struct Size2u {
    static_assert(std::same_as<unsigned, uint32_t>);

    uint32_t width;
    uint32_t height;

    constexpr explicit(false) operator vk::Extent2D() const noexcept
    {
        return vk::Extent2D{ .width = width, .height = height };
    }
};

}   // namespace kiln::wsi
