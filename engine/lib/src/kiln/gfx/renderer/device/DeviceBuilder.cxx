module;

#include <algorithm>
#include <memory_resource>
#include <ranges>
#include <span>
#include <utility>

module kiln.gfx.renderer.device.DeviceBuilder;

import vulkan_hpp;

import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.DeviceBuildFailedError;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.QueueInfo;
import kiln.gfx.vulkan.result.check_result;
import kiln.gfx.vulkan.structure_chain.StructureChain;
import kiln.wsi.vulkan_queue_family_supports_presenting;

namespace kiln::gfx::renderer {

auto DeviceBuilder::create(app::MemoryArena& memory_arena) -> DeviceBuilder
{
    return DeviceBuilder{ memory_arena.pool_allocator() };
}

DeviceBuilder::DeviceBuilder(const DeviceBuilder& other, const allocator_type& allocator)
    : m_physical_device_filter{ other.m_physical_device_filter, allocator },
      m_optional_capabilities{ other.m_optional_capabilities, allocator }
{
}

DeviceBuilder::DeviceBuilder(DeviceBuilder&& other, const allocator_type& allocator)
    : m_physical_device_filter{ std::move(other.m_physical_device_filter), allocator },
      m_optional_capabilities{ std::move(other.m_optional_capabilities), allocator }
{
}

DeviceBuilder::DeviceBuilder(const allocator_type& allocator)
    : m_physical_device_filter{ allocator },
      m_optional_capabilities{ allocator }
{
}

DeviceBuilder::DeviceBuilder(vulkan::PhysicalDeviceFilter&& physical_device_selector)
    : DeviceBuilder{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          std::move(physical_device_selector),
      }
{
}

DeviceBuilder::DeviceBuilder(
    std::allocator_arg_t,
    const allocator_type&          allocator,
    vulkan::PhysicalDeviceFilter&& physical_device_selector
)
    : m_physical_device_filter{ std::move(physical_device_selector), allocator }
{
}

auto DeviceBuilder::get_allocator() const -> allocator_type
{
    return m_physical_device_filter.get_allocator();
}

auto device_queue_create_infos_from(
    const std::span<const vulkan::QueueFamilyInfo> family_infos,
    const std::pmr::polymorphic_allocator<>&       allocator
) -> std::pmr::vector<vk::DeviceQueueCreateInfo>
{
    std::pmr::vector<vk::DeviceQueueCreateInfo> result{ allocator };
    result.reserve(
        std::ranges::fold_left(
            std::views::transform(family_infos, &vulkan::QueueFamilyInfo::queue_count),
            0,
            std::plus{}
        )
    );

    for (const vulkan::QueueFamilyInfo& family_info : family_infos)
    {
        for (const auto& create_info : family_info.create_infos())
        {
            result.push_back(static_cast<const vk::DeviceQueueCreateInfo&>(create_info));
        }
    }

    return result;
}

auto DeviceBuilder::request_queue(const QueueType type) -> void
{
    m_requested_queue_types |= type;
}

auto DeviceBuilder::build(
    app::MemoryArena&       memory_arena,
    const vulkan::Instance& instance,
    const wsi::Context&     wsi_context
) const -> Device
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

    Device::QueueInfos queue_infos;
    std::pmr::vector   queue_family_infos{
        create_queue_family_infos(
            instance,
            wsi_context,
            physical_device,
            queue_infos,
            memory_arena.pool_allocator()
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
        std::move(queue_family_infos),
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
    vulkan::QueueFamilyInfo&         out,
    const vk::DeviceQueueCreateFlags flags    = {},
    const float                      priority = 0.5f
) -> std::optional<vulkan::QueueInfo>
{
    return out.try_emplace(flags, priority);
}

auto try_emplace_graphics_queue(const std::span<vulkan::QueueFamilyInfo> out)
    -> std::optional<vulkan::QueueInfo>
{
    for (vulkan::QueueFamilyInfo& queue_family_info : out)
    {
        if (queue_family_info.flags() & vk::QueueFlagBits::eGraphics
            // TODO: move presentation support logic elsewhere
            && queue_family_info.supports_presentation())
        {
            return try_emplace_queue(queue_family_info);
        }
    }

    return std::nullopt;
}

[[nodiscard]]
auto is_dedicated_compute_queue_family(const vk::QueueFlags flags) -> bool
{
    return flags & vk::QueueFlagBits::eCompute && !(flags & vk::QueueFlagBits::eGraphics);
}

auto try_emplace_compute_queue(const std::span<vulkan::QueueFamilyInfo> out)
    -> std::optional<vulkan::QueueInfo>
{
    for (vulkan::QueueFamilyInfo& queue_family_info : out)
    {
        if (is_dedicated_compute_queue_family(queue_family_info.flags()))
        {
            return try_emplace_queue(queue_family_info);
        }
    }

    for (vulkan::QueueFamilyInfo& queue_family_info : out)
    {
        if (queue_family_info.flags() & vk::QueueFlagBits::eCompute)
        {
            return try_emplace_queue(queue_family_info);
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

auto try_emplace_transfer_queue(const std::span<vulkan::QueueFamilyInfo> out)
    -> std::optional<vulkan::QueueInfo>
{
    for (vulkan::QueueFamilyInfo& queue_family_info : out)
    {
        if (is_dedicated_transfer_queue_family(queue_family_info.flags()))
        {
            return try_emplace_queue(queue_family_info);
        }
    }

    for (vulkan::QueueFamilyInfo& queue_family_info : out)
    {
        if (queue_family_info.flags() & vk::QueueFlagBits::eTransfer)
        {
            return try_emplace_queue(queue_family_info);
        }
    }

    return std::nullopt;
}

auto DeviceBuilder::create_queue_family_infos(
    const vulkan::Instance&                  instance,
    const wsi::Context&                      wsi_context,
    const vk::raii::PhysicalDevice&          physical_device,
    Device::QueueInfos&                      out_queue_infos,
    const std::pmr::polymorphic_allocator<>& allocator
) const -> std::pmr::vector<vulkan::QueueFamilyInfo>
{
    const auto queue_family_properties{ physical_device.getQueueFamilyProperties2() };

    std::pmr::vector<vulkan::QueueFamilyInfo> result{ allocator };
    result.reserve(queue_family_properties.size());
    for (auto&& [index, properties] : std::views::enumerate(queue_family_properties))
    {
        const vulkan::QueueFamilyIndex family_index{
            static_cast<vulkan::QueueFamilyIndex::Underlying>(index)
        };
        result.emplace_back(
            family_index,
            properties,
            wsi::vulkan_queue_family_supports_presenting(
                wsi_context,
                instance.get(),
                physical_device,
                family_index
            )
        );
    }

    if (m_requested_queue_types & QueueType::eGraphics)
    {
        out_queue_infos.graphics_queue_info = try_emplace_graphics_queue(result);
    }
    if (m_requested_queue_types & QueueType::eCompute)
    {
        out_queue_infos.compute_queue_info = try_emplace_compute_queue(result);
    }
    if (m_requested_queue_types & QueueType::eHostToDeviceTransfer)
    {
        out_queue_infos.host_to_device_transfer_queue_info
            = try_emplace_transfer_queue(result);
    }

    return result;
}

}   // namespace kiln::gfx::renderer
