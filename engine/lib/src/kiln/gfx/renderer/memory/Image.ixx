export module kiln.gfx.renderer.memory.Image;

import vulkan;

import kiln.gfx.renderer.memory.Allocation;

namespace kiln::gfx::renderer {

export class Image {
public:
    explicit Image() = default;
    explicit Image(
        vk::raii::Image&& image,
        vk::Format        format,
        Allocation&&      allocation
    ) noexcept;


    [[nodiscard]]
    auto get() noexcept -> vk::Image;
    [[nodiscard]]
    auto format() const noexcept -> vk::Format;
    [[nodiscard]]
    auto allocation() noexcept -> Allocation&;
    [[nodiscard]]
    auto allocation() const noexcept -> const Allocation&;

private:
    vk::raii::Image m_image{ nullptr };
    vk::Format      m_format{};
    Allocation      m_allocation;
};

}   // namespace kiln::gfx::renderer
