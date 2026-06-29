module;

#include <utility>

module kiln.gfx.renderer.memory.Image;

import vulkan;

namespace kiln::gfx::renderer {

Image::Image(
    vk::raii::Image&& image,
    const vk::Format  format,
    Allocation&&      allocation
) noexcept
    : m_image{ std::move(image) },
      m_format{ format },
      m_allocation{ std::move(allocation) }
{
}

// ReSharper disable once CppMemberFunctionMayBeConst
auto Image::get() noexcept -> vk::Image
{
    return *m_image;
}

auto Image::format() const noexcept -> vk::Format
{
    return m_format;
}

auto Image::allocation() noexcept -> Allocation&
{
    return m_allocation;
}

auto Image::allocation() const noexcept -> const Allocation&
{
    return m_allocation;
}

}   // namespace kiln::gfx::renderer
