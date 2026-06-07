export module kiln.gfx.vulkan.context;

import vulkan_hpp;

namespace kiln::gfx::vulkan {

export [[nodiscard]]
auto context() -> const vk::raii::Context&;

}   // namespace kiln::gfx::vulkan

module :private;

auto kiln::gfx::vulkan::context() -> const vk::raii::Context&
{
    static const vk::raii::Context static_context;

    return static_context;
}
