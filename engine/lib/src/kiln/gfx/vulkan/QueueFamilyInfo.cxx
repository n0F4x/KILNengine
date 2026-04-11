module;

#include <memory_resource>

module kiln.gfx.vulkan.QueueFamilyInfo;

namespace kiln::gfx::vulkan {

QueueFamilyInfo::QueueFamilyInfo(
    const QueueFamilyInfo& other,
    const allocator_type&  allocator
)
    : m_info{ other.m_info, allocator },
      m_priorities{ other.m_priorities, allocator }
{
}

QueueFamilyInfo::QueueFamilyInfo(QueueFamilyInfo&& other, const allocator_type& allocator)
    : m_info{ std::move(other.m_info), allocator },
      m_priorities{ std::move(other.m_priorities), allocator }
{
}

QueueFamilyInfo::QueueFamilyInfo(
    const QueueFamilyIndex           index,
    const vk::DeviceQueueCreateFlags flags
)
    : QueueFamilyInfo{ std::allocator_arg, std::pmr::get_default_resource(), index, flags }
{
}

QueueFamilyInfo::QueueFamilyInfo(
    std::allocator_arg_t,
    const allocator_type&            allocator,
    const QueueFamilyIndex           index,
    const vk::DeviceQueueCreateFlags flags
)
    : m_info{ allocator },
      m_priorities{ allocator }
{
    m_info.root().flags            = flags;
    m_info.root().queueFamilyIndex = index.underlying();
}

QueueFamilyInfo::operator const vk::DeviceQueueCreateInfo&() const noexcept
{
    return m_info.root();
}

auto QueueFamilyInfo::get_allocator() const noexcept -> allocator_type
{
    return m_info.get_allocator();
}

auto QueueFamilyInfo::index() const noexcept -> QueueFamilyIndex
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

}   // namespace kiln::gfx::vulkan
