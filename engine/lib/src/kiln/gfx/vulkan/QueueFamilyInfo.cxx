module;

#include <algorithm>
#include <memory_resource>
#include <optional>
#include <ranges>
#include <utility>

module kiln.gfx.vulkan.QueueFamilyInfo;

namespace kiln::gfx::vulkan {

DeviceCreateInfo::DeviceCreateInfo(
    const DeviceCreateInfo& other,
    const allocator_type&   allocator
)
    : m_info{ other.m_info, allocator },
      m_priorities{ other.m_priorities, allocator }
{
}

DeviceCreateInfo::DeviceCreateInfo(
    DeviceCreateInfo&&    other,
    const allocator_type& allocator
)
    : m_info{ std::move(other.m_info), allocator },
      m_priorities{ std::move(other.m_priorities), allocator }
{
}

DeviceCreateInfo::DeviceCreateInfo(
    const QueueFamilyIndex           family_index,
    const vk::DeviceQueueCreateFlags flags
)
    : DeviceCreateInfo{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          family_index,
          flags,
      }
{
}

DeviceCreateInfo::DeviceCreateInfo(
    std::allocator_arg_t,
    const allocator_type&            allocator,
    const QueueFamilyIndex           family_index,
    const vk::DeviceQueueCreateFlags flags
)
    : m_info{ allocator },
      m_priorities{ allocator }
{
    m_info.root().flags            = flags;
    m_info.root().queueFamilyIndex = family_index.underlying();
}

DeviceCreateInfo::operator const vk::DeviceQueueCreateInfo&() const noexcept
{
    return m_info.root();
}

auto DeviceCreateInfo::get_allocator() const noexcept -> allocator_type
{
    return m_info.get_allocator();
}

auto DeviceCreateInfo::flags() const noexcept -> vk::DeviceQueueCreateFlags
{
    return m_info.root().flags;
}

auto DeviceCreateInfo::queue_count() const noexcept -> uint32_t
{
    return m_info.root().queueCount;
}

auto DeviceCreateInfo::emplace_back(const float priority) -> void
{
    m_priorities.push_back(priority);
    ++m_info.root().queueCount;
    m_info.root().pQueuePriorities = m_priorities.data();
}

QueueFamilyInfo::QueueFamilyInfo(
    const QueueFamilyInfo& other,
    const allocator_type&  allocator
)
    : m_family_index{ other.m_family_index },
      m_max_queue_count{ other.max_queue_count() },
      m_flags{ other.m_flags },
      m_supports_presentation{ other.m_supports_presentation },
      m_create_infos{ other.m_create_infos, allocator }
{
}

QueueFamilyInfo::QueueFamilyInfo(QueueFamilyInfo&& other, const allocator_type& allocator)
    : m_family_index{ other.m_family_index },
      m_max_queue_count{ other.max_queue_count() },
      m_flags{ other.m_flags },
      m_supports_presentation{ other.m_supports_presentation },
      m_create_infos{ std::move(other.m_create_infos), allocator }
{
}

QueueFamilyInfo::QueueFamilyInfo(
    const QueueFamilyIndex            family_index,
    const vk::QueueFamilyProperties2& properties,
    const bool                        supports_presentation
)
    : QueueFamilyInfo{
          std::allocator_arg,
          std::pmr::get_default_resource(),   //
          family_index,
          properties,
          supports_presentation,
      }
{
}

QueueFamilyInfo::QueueFamilyInfo(
    std::allocator_arg_t,
    const allocator_type&             allocator,
    const QueueFamilyIndex            family_index,
    const vk::QueueFamilyProperties2& properties,
    const bool                        supports_presentation
)
    : m_family_index{ family_index },
      m_max_queue_count{ properties.queueFamilyProperties.queueCount },
      m_flags{ properties.queueFamilyProperties.queueFlags },
      m_supports_presentation{ supports_presentation },
      m_create_infos{ allocator }
{
}

auto QueueFamilyInfo::get_allocator() const noexcept -> allocator_type
{
    return m_create_infos.get_allocator();
}

auto QueueFamilyInfo::index() const noexcept -> QueueFamilyIndex
{
    return m_family_index;
}

auto QueueFamilyInfo::queue_count() const noexcept -> uint32_t
{
    return std::ranges::fold_left(
        std::views::transform(m_create_infos, &DeviceCreateInfo::queue_count),
        0,
        std::plus{}
    );
}

auto QueueFamilyInfo::max_queue_count() const noexcept -> uint32_t
{
    return m_max_queue_count;
}

auto QueueFamilyInfo::flags() const noexcept -> vk::QueueFlags
{
    return m_flags;
}

auto QueueFamilyInfo::supports_presentation() const noexcept -> bool
{
    return m_supports_presentation;
}

auto QueueFamilyInfo::full() const noexcept -> bool
{
    return queue_count() == max_queue_count();
}

auto QueueFamilyInfo::try_create_info(
    const vk::DeviceQueueCreateFlags flags
) const noexcept -> util::OptionalRef<const DeviceCreateInfo>
{
    for (const DeviceCreateInfo& create_info : m_create_infos)
    {
        if (create_info.flags() == flags)
        {
            return create_info;
        }
    }
    return std::nullopt;
}

auto QueueFamilyInfo::create_infos() const noexcept -> std::span<const DeviceCreateInfo>
{
    return m_create_infos;
}

auto QueueFamilyInfo::try_emplace(
    const vk::DeviceQueueCreateFlags flags,
    const float                      priority
) -> std::optional<QueueInfo>
{
    if (full())
    {
        return std::nullopt;
    }

    DeviceCreateInfo& create_info = try_emplace_create_info(flags);
    create_info.emplace_back(priority);

    return QueueInfo{
        .family_index = index(),
        .flags        = flags,
        .index        = try_create_info(flags)->queue_count() - 1,
    };
}

auto QueueFamilyInfo::try_emplace_create_info(const vk::DeviceQueueCreateFlags flags)
    -> DeviceCreateInfo&
{
    if (const auto iter
        = std::ranges::find(m_create_infos, flags, &DeviceCreateInfo::flags);
        iter != m_create_infos.cend())
    {
        return *iter;
    }

    return m_create_infos.emplace_back(m_family_index, flags);
}

}   // namespace kiln::gfx::vulkan
