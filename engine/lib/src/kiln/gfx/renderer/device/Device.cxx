module;

#include <algorithm>
#include <memory_resource>
#include <ranges>
#include <span>
#include <string>
#include <utility>
#include <vector>

module kiln.gfx.renderer.device.Device;

import kiln.app.memory.MemoryArena;
import kiln.gfx.renderer.device.DeviceBuildFailedError;
import kiln.gfx.renderer.device.QueueType;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.result.check_result;
import kiln.gfx.vulkan.structure_chain.StructureChain;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.QueueFamilyInfo;
import kiln.wsi.Context;
import kiln.wsi.vulkan_queue_family_supports_presenting;

namespace kiln::gfx::renderer {

Device::Device(Device&& other, const allocator_type& allocator)
    : m_physical_device{ std::move(other.m_physical_device) },
      m_logical_device{ std::move(other.m_logical_device) },
      m_capabilities{ std::move(other.m_capabilities), allocator },
      m_queue_infos{ other.m_queue_infos }
{
}

Device::Device(
    vk::raii::PhysicalDevice&&           physical_device,
    vk::raii::Device&&                   logical_device,
    vulkan::PhysicalDeviceCapabilities&& capabilities,
    const QueueInfos&                    queue_infos
)
    : Device{
          std::allocator_arg,   //
          std::pmr::get_default_resource(),
          std::move(physical_device),
          std::move(logical_device),
          std::move(capabilities),
          queue_infos,
      }
{
}

Device::Device(
    std::allocator_arg_t,
    const allocator_type&                allocator,
    vk::raii::PhysicalDevice&&           physical_device,
    vk::raii::Device&&                   logical_device,
    vulkan::PhysicalDeviceCapabilities&& capabilities,
    const QueueInfos&                    queue_infos
)
    : m_physical_device{ std::move(physical_device) },
      m_logical_device{ std::move(logical_device) },
      m_capabilities{ std::move(capabilities), allocator },
      m_queue_infos{ queue_infos }
{
}

auto Device::name() const -> std::string
{
    return m_physical_device.getProperties2().properties.deviceName;
}

auto Device::physical_device() const noexcept -> const vk::raii::PhysicalDevice&
{
    return m_physical_device;
}

auto Device::logical_device() const noexcept -> const vk::raii::Device&
{
    return m_logical_device;
}

auto Device::capabilities() const noexcept -> const vulkan::PhysicalDeviceCapabilities&
{
    return m_capabilities;
}

auto Device::graphics_queue_info() const noexcept
    -> util::OptionalRef<const vulkan::QueueInfo>
{
    if (m_queue_infos.graphics_queue_info.has_value())
    {
        return *m_queue_infos.graphics_queue_info;
    }
    return std::nullopt;
}

auto Device::compute_queue_info() const noexcept
    -> util::OptionalRef<const vulkan::QueueInfo>
{
    if (m_queue_infos.compute_queue_info.has_value())
    {
        return *m_queue_infos.compute_queue_info;
    }
    return std::nullopt;
}

auto Device::host_to_device_transfer_queue_info() const noexcept
    -> util::OptionalRef<const vulkan::QueueInfo>
{
    if (m_queue_infos.host_to_device_transfer_queue_info.has_value())
    {
        return *m_queue_infos.host_to_device_transfer_queue_info;
    }
    return std::nullopt;
}

auto Device::Builder::create(app::MemoryArenaBuilder& memory_arena_builder) -> Builder
{
    return Builder{ memory_arena_builder.arena().pool_allocator() };
}

Device::Builder::Builder(const Builder& other, const allocator_type& allocator)
    : m_physical_device_filter{ other.m_physical_device_filter, allocator },
      m_optional_capabilities{ other.m_optional_capabilities, allocator }
{
}

Device::Builder::Builder(Builder&& other, const allocator_type& allocator)
    : m_physical_device_filter{ std::move(other.m_physical_device_filter), allocator },
      m_optional_capabilities{ std::move(other.m_optional_capabilities), allocator }
{
}

Device::Builder::Builder(const allocator_type& allocator)
    : m_physical_device_filter{ allocator },
      m_optional_capabilities{ allocator }
{
}

Device::Builder::Builder(vulkan::PhysicalDeviceFilter&& physical_device_selector)
    : Builder{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          std::move(physical_device_selector),
      }
{
}

Device::Builder::Builder(
    std::allocator_arg_t,
    const allocator_type&          allocator,
    vulkan::PhysicalDeviceFilter&& physical_device_selector
)
    : m_physical_device_filter{ std::move(physical_device_selector), allocator }
{
}

auto Device::Builder::get_allocator() const -> allocator_type
{
    return m_physical_device_filter.get_allocator();
}

auto device_queue_create_infos_from(
    const std::span<const vulkan::QueueFamilyInfo> family_infos,
    const std::pmr::polymorphic_allocator<>&       allocator
) -> std::pmr::vector<vk::DeviceQueueCreateInfo>
{
    std::pmr::vector<vk::DeviceQueueCreateInfo> result{ allocator };
    result.reserve(family_infos.size());

    for (const vulkan::QueueFamilyInfo& family_info : family_infos)
    {
        result.push_back(static_cast<const vk::DeviceQueueCreateInfo&>(family_info));
    }

    return result;
}

auto Device::Builder::request_queue(const QueueType type) -> void
{
    m_requested_queue_types |= type;
}

auto Device::Builder::build(
    app::MemoryArena&       memory_arena,
    const vulkan::Instance& instance,
    const wsi::Context&     wsi_context
) const&& -> Device
{
    std::pmr::monotonic_buffer_resource transient_memory_resource{
        memory_arena.make_transient_resource()
    };

    std::vector<vk::raii::PhysicalDevice> supported_devices{
        vulkan::check_result(instance.get().enumeratePhysicalDevices())
    };
    std::erase_if(
        supported_devices,
        [this](const vk::raii::PhysicalDevice& physical_device) -> bool
        {
            return !m_physical_device_filter.is_adequate(physical_device);   //
        }
    );

    if (supported_devices.empty())
    {
        throw DeviceBuildFailedError{ "No supported device found" };
    }

    vk::raii::PhysicalDevice physical_device{ std::move(supported_devices.front()) };

    vulkan::PhysicalDeviceCapabilities capabilities{
        m_physical_device_filter.required_capabilities()
    };

    for (const std::vector<vk::ExtensionProperties> supported_extension_properties{
             physical_device.enumerateDeviceExtensionProperties() };
         const util::StringLiteral optional_extension :
         m_optional_capabilities.extensions())
    {
        if (std::ranges::any_of(
                supported_extension_properties,
                [optional_extension](const vk::ExtensionProperties& extension_properties)
                    -> bool
                {
                    return optional_extension
                        == util::StringLiteral::unsafe_create(
                               extension_properties.extensionName
                        );
                }
            ))
        {
            capabilities.insert_extension(optional_extension);
        }
    }

    vulkan::StructureChain<vk::PhysicalDeviceFeatures2> supported_optional_features{
        m_optional_capabilities.features_chain(),
        &transient_memory_resource
    };
    vulkan::StructureChain<vk::PhysicalDeviceFeatures2> optional_feature_support{
        m_optional_capabilities.features_chain(),
        &transient_memory_resource
    };
    physical_device.getDispatcher()
        ->vkGetPhysicalDeviceFeatures2(*physical_device, optional_feature_support.root());
    supported_optional_features.erase_unsupported_features(
        optional_feature_support.root()
    );
    capabilities.insert_features(supported_optional_features);

    QueueInfos             queue_infos;
    // TODO: use std::inplace_vector
    const std::pmr::vector queue_family_infos{
        create_queue_family_infos(
            instance,
            wsi_context,
            physical_device,
            queue_infos,
            &transient_memory_resource
        )   //
    };
    const std::pmr::vector queue_create_infos{
        device_queue_create_infos_from(queue_family_infos, &transient_memory_resource)
    };

    const vk::DeviceCreateInfo device_create_info{
        .pNext                 = &capabilities.features_chain().root(),
        .queueCreateInfoCount  = static_cast<uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos     = queue_create_infos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(capabilities.extensions().size()),
        .ppEnabledExtensionNames = capabilities.extensions().empty()
                                     ? nullptr
                                     : capabilities.extensions().front().address(),
    };

    vk::raii::Device device{
        vulkan::check_result(physical_device.createDevice(device_create_info))
    };

    return Device{
        std::allocator_arg,   //
        memory_arena.pool_allocator(),
        std::move(physical_device),
        std::move(device),
        std::move(capabilities),
        queue_infos,
    };
}

[[nodiscard]]
auto queue_family_is_full(
    const std::span<const vulkan::QueueFamilyInfo> family_infos,
    const uint32_t                                 capacity,
    const vulkan::QueueFamilyIndex                 family_index
) -> bool
{
    uint32_t count{};

    for (const vulkan::QueueFamilyInfo& family_info : family_infos)
    {
        if (family_info.index() == family_index)
        {
            count += family_info.queue_count();
        }
    }

    return count >= capacity;
}

auto try_emplace_queue(
    std::pmr::vector<vulkan::QueueFamilyInfo>& out,
    const uint32_t                             queue_family_capacity,
    const vulkan::QueueFamilyIndex             queue_family_index,
    const vk::DeviceQueueCreateFlags           flags    = {},
    const float                                priority = 0.5f
) -> std::optional<uint32_t>
{
    if (queue_family_is_full(out, queue_family_capacity, queue_family_index))
    {
        return std::nullopt;
    }

    if (const auto iter{
            std::ranges::find_if(
                out,
                [queue_family_index,
                 flags](const vulkan::QueueFamilyInfo& family_info) -> bool
                {
                    return family_info.index() == queue_family_index
                        && family_info.flags() == flags;
                }
            )   //
        };
        iter != out.cend())
    {
        iter->emplace_back(priority);
        return iter->queue_count() - 1;
    }

    vulkan::QueueFamilyInfo& queue_family_info
        = out.emplace_back(queue_family_index, flags);
    queue_family_info.emplace_back(priority);
    return queue_family_info.queue_count() - 1;
}

auto emplace_graphics_queue(
    std::pmr::vector<vulkan::QueueFamilyInfo>& out,
    const vulkan::Instance&                    instance,
    const wsi::Context&                        wsi_context,
    const vk::raii::PhysicalDevice&            physical_device
) -> std::optional<vulkan::QueueInfo>
{
    for (auto&& [family_index, family_properties] : std::views::zip(
             std::views::iota(uint32_t{}),
             physical_device.getQueueFamilyProperties2()
         ))
    {
        // TODO: move presentation support logic elsewhere
        if (family_properties.queueFamilyProperties.queueFlags
                & vk::QueueFlagBits::eGraphics
            && wsi::vulkan_queue_family_supports_presenting(
                wsi_context,
                instance.get(),
                physical_device,
                vulkan::QueueFamilyIndex{ family_index }
            ))
        {
            if (const std::optional<uint32_t> queue_index = try_emplace_queue(
                    out,
                    family_properties.queueFamilyProperties.queueCount,
                    vulkan::QueueFamilyIndex{ family_index }
                );
                queue_index.has_value())
            {
                return vulkan::QueueInfo{
                    .family_index = vulkan::QueueFamilyIndex{ family_index },
                    .flags        = family_properties.queueFamilyProperties.queueFlags,
                    .index        = *queue_index,
                };
            }
        }
    }

    return std::nullopt;
}

[[nodiscard]]
auto is_dedicated_compute_queue_family(const vk::QueueFlags flags) -> bool
{
    return flags & vk::QueueFlagBits::eCompute && !(flags & vk::QueueFlagBits::eGraphics);
}

auto emplace_compute_queue(
    std::pmr::vector<vulkan::QueueFamilyInfo>& out,
    const vk::raii::PhysicalDevice&            physical_device
) -> std::optional<vulkan::QueueInfo>
{
    const std::vector<vk::QueueFamilyProperties2> queue_family_properties{
        physical_device.getQueueFamilyProperties2()
    };

    for (auto&& [family_index, family_properties] :
         std::views::zip(std::views::iota(uint32_t{}), queue_family_properties))
    {
        if (is_dedicated_compute_queue_family(
                family_properties.queueFamilyProperties.queueFlags
            ))
        {
            if (const std::optional<uint32_t> queue_index = try_emplace_queue(
                    out,
                    family_properties.queueFamilyProperties.queueCount,
                    vulkan::QueueFamilyIndex{ family_index }
                );
                queue_index.has_value())
            {
                return vulkan::QueueInfo{
                    .family_index = vulkan::QueueFamilyIndex{ family_index },
                    .flags        = family_properties.queueFamilyProperties.queueFlags,
                    .index        = *queue_index,
                };
            }
        }
    }

    for (auto&& [family_index, family_properties] :
         std::views::zip(std::views::iota(uint32_t{}), queue_family_properties))
    {
        if (family_properties.queueFamilyProperties.queueFlags
            & vk::QueueFlagBits::eCompute)
        {
            if (const std::optional<uint32_t> queue_index = try_emplace_queue(
                    out,
                    family_properties.queueFamilyProperties.queueCount,
                    vulkan::QueueFamilyIndex{ family_index }
                );
                queue_index.has_value())
            {
                return vulkan::QueueInfo{
                    .family_index = vulkan::QueueFamilyIndex{ family_index },
                    .flags        = family_properties.queueFamilyProperties.queueFlags,
                    .index        = *queue_index,
                };
            }
        }
    }

    return std::nullopt;
}

[[nodiscard]]
auto is_dedicated_transfer_queue_family(const vk::QueueFlags flags) -> bool
{
    return flags & vk::QueueFlagBits::eTransfer
        && !(flags & vk::QueueFlagBits::eGraphics)
        && !(flags & vk::QueueFlagBits::eCompute);
}

auto emplace_transfer_queue(
    std::pmr::vector<vulkan::QueueFamilyInfo>& out,
    const vk::raii::PhysicalDevice&            physical_device
) -> std::optional<vulkan::QueueInfo>
{
    const std::vector<vk::QueueFamilyProperties2> queue_family_properties{
        physical_device.getQueueFamilyProperties2()
    };

    for (auto&& [family_index, family_properties] :
         std::views::zip(std::views::iota(uint32_t{}), queue_family_properties))
    {
        if (is_dedicated_transfer_queue_family(
                family_properties.queueFamilyProperties.queueFlags
            ))
        {
            if (const std::optional<uint32_t> queue_index = try_emplace_queue(
                    out,
                    family_properties.queueFamilyProperties.queueCount,
                    vulkan::QueueFamilyIndex{ family_index }
                );
                queue_index.has_value())
            {
                return vulkan::QueueInfo{
                    .family_index = vulkan::QueueFamilyIndex{ family_index },
                    .flags        = family_properties.queueFamilyProperties.queueFlags,
                    .index        = *queue_index,
                };
            }
        }
    }

    for (auto&& [family_index, family_properties] :
         std::views::zip(std::views::iota(uint32_t{}), queue_family_properties))
    {
        if (family_properties.queueFamilyProperties.queueFlags
            & vk::QueueFlagBits::eTransfer)
        {
            if (const std::optional<uint32_t> queue_index = try_emplace_queue(
                    out,
                    family_properties.queueFamilyProperties.queueCount,
                    vulkan::QueueFamilyIndex{ family_index }
                );
                queue_index.has_value())
            {
                return vulkan::QueueInfo{
                    .family_index = vulkan::QueueFamilyIndex{ family_index },
                    .flags        = family_properties.queueFamilyProperties.queueFlags,
                    .index        = *queue_index,
                };
            }
        }
    }

    return std::nullopt;
}

auto Device::Builder::create_queue_family_infos(
    const vulkan::Instance&                  instance,
    const wsi::Context&                      wsi_context,
    const vk::raii::PhysicalDevice&          physical_device,
    QueueInfos&                              out_queue_infos,
    const std::pmr::polymorphic_allocator<>& allocator
) const -> std::pmr::vector<vulkan::QueueFamilyInfo>
{
    std::pmr::vector<vulkan::QueueFamilyInfo> result{ allocator };

    if (m_requested_queue_types & QueueType::eGraphics)
    {
        out_queue_infos.graphics_queue_info
            = emplace_graphics_queue(result, instance, wsi_context, physical_device);
    }
    if (m_requested_queue_types & QueueType::eCompute)
    {
        out_queue_infos.compute_queue_info
            = emplace_compute_queue(result, physical_device);
    }
    if (m_requested_queue_types & QueueType::eHostToDeviceTransfer)
    {
        out_queue_infos.host_to_device_transfer_queue_info
            = emplace_transfer_queue(result, physical_device);
    }

    return result;
}

}   // namespace kiln::gfx::renderer
