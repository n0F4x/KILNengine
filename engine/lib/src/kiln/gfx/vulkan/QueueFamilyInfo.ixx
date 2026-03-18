module;

#include <cstdint>
#include <vector>

export module kiln.gfx.vulkan.QueueFamilyInfo;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.structure_chain.StructureChain;
import kiln.util.contracts;
import kiln.util.type_traits.const_like;

namespace kiln::gfx::vulkan {

export class QueueFamilyInfo {
public:
    explicit QueueFamilyInfo(QueueFamilyIndex index, vk::DeviceQueueCreateFlags flags = {});

    [[nodiscard]]
    auto index() const noexcept -> QueueFamilyIndex;

    [[nodiscard]]
    auto flags() const noexcept -> vk::DeviceQueueCreateFlags;

    [[nodiscard]]
    auto queue_count() const noexcept -> uint32_t;

    auto emplace_back(float priority = 0.5f) -> void;

    explicit operator const vk::DeviceQueueCreateInfo&() const noexcept;

private:
    StructureChain<vk::DeviceQueueCreateInfo> m_info;
    std::vector<float>                        m_priorities;
};

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan {

QueueFamilyInfo::QueueFamilyInfo(
    const QueueFamilyIndex           index,
    const vk::DeviceQueueCreateFlags flags
)
{
    m_info.root().flags            = flags;
    m_info.root().queueFamilyIndex = index.underlying();
}

inline auto QueueFamilyInfo::index() const noexcept -> QueueFamilyIndex
{
    return QueueFamilyIndex{ m_info.root().queueFamilyIndex };
}

auto QueueFamilyInfo::flags() const noexcept -> vk::DeviceQueueCreateFlags
{
    return m_info.root().flags;
}

auto QueueFamilyInfo::queue_count() const noexcept -> uint32_t
{
    return m_info.root().queueCount;
}

auto QueueFamilyInfo::emplace_back(const float priority) -> void
{
    m_priorities.push_back(priority);
    ++m_info.root().queueCount;
    m_info.root().pQueuePriorities = m_priorities.data();
}

QueueFamilyInfo::operator const vk::DeviceQueueCreateInfo&() const noexcept
{
    return m_info.root();
}

}   // namespace kiln::gfx::vulkan
