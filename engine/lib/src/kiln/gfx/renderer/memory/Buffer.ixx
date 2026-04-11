export module kiln.gfx.renderer.memory.Buffer;

import vulkan_hpp;

import kiln.gfx.renderer.memory.Allocation;

namespace kiln::gfx::renderer {

export class Buffer {
public:
    explicit Buffer(
        vk::raii::Buffer&& buffer,
        vk::DeviceSize     buffer_size,
        Allocation&&       allocation
    ) noexcept;


    [[nodiscard]]
    auto get() noexcept -> vk::Buffer;
    [[nodiscard]]
    auto size() const noexcept -> vk::DeviceSize;
    [[nodiscard]]
    auto allocation() noexcept -> Allocation&;
    [[nodiscard]]
    auto allocation() const noexcept -> const Allocation&;

private:
    vk::raii::Buffer m_buffer;
    vk::DeviceSize   m_size{};
    Allocation       m_allocation;
};

}   // namespace kiln::gfx::renderer
