module;

#include <cstdint>
#include <memory_resource>
#include <vector>

export module kiln.gfx.vulkan.QueueFamilyInfo;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.structure_chain.StructureChain;

namespace kiln::gfx::vulkan {

export class QueueFamilyInfo {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    QueueFamilyInfo(const QueueFamilyInfo&, const allocator_type& allocator);
    QueueFamilyInfo(QueueFamilyInfo&&, const allocator_type& allocator);

    explicit QueueFamilyInfo(QueueFamilyIndex index, vk::DeviceQueueCreateFlags flags = {});
    explicit QueueFamilyInfo(
        std::allocator_arg_t,
        const allocator_type&      allocator,
        QueueFamilyIndex           index,
        vk::DeviceQueueCreateFlags flags = {}
    );

    explicit operator const vk::DeviceQueueCreateInfo&() const noexcept;

    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;
    [[nodiscard]]
    auto index() const noexcept -> QueueFamilyIndex;
    [[nodiscard]]
    auto flags() const noexcept -> vk::DeviceQueueCreateFlags;
    [[nodiscard]]
    auto queue_count() const noexcept -> uint32_t;

    auto emplace_back(float priority = 0.5f) -> void;

private:
    StructureChain<vk::DeviceQueueCreateInfo> m_info;
    std::pmr::vector<float>                   m_priorities;
};

}   // namespace kiln::gfx::vulkan
