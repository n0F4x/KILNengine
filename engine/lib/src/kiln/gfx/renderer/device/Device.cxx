module;

#include <algorithm>
#include <memory_resource>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "kiln/util/lifetimebound.hpp"

module kiln.gfx.renderer.device.Device;

import kiln.gfx.renderer.device.DeviceBuildFailedError;
import kiln.gfx.renderer.device.ErasedQueueRequest;
import kiln.gfx.vulkan.result.check_result;
import kiln.gfx.vulkan.structure_chain.StructureChain;
import kiln.gfx.vulkan.QueueFamilyInfo;

namespace kiln::gfx::renderer {

Device::Device(
    vk::raii::PhysicalDevice           physical_device,
    vk::raii::Device                   logical_device,
    vulkan::PhysicalDeviceCapabilities capabilities
)
    : m_physical_device{ std::move(physical_device) },
      m_logical_device{ std::move(logical_device) },
      m_capabilities{ std::move(capabilities) }
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

namespace internal {

auto DeviceBuilder::create(app::ArenaBuilder& arena_builder) -> DeviceBuilder
{
    return DeviceBuilder{ arena_builder.arena().pool_allocator() };
}

DeviceBuilder::DeviceBuilder(const allocator_type& allocator)
    : m_queue_requests{ allocator }
{
}

DeviceBuilder::DeviceBuilder(const DeviceBuilder& other, const allocator_type& allocator)
    : m_physical_device_filter{ other.m_physical_device_filter },
      m_optional_capabilities{ other.m_optional_capabilities },
      m_queue_requests{ other.m_queue_requests, allocator }
{
}

DeviceBuilder::DeviceBuilder(DeviceBuilder&& other, const allocator_type& allocator)
    : m_physical_device_filter{ std::move(other.m_physical_device_filter) },
      m_optional_capabilities{ std::move(other.m_optional_capabilities) },
      m_queue_requests{ std::move(other.m_queue_requests), allocator }
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
    : m_physical_device_filter{ std::move(physical_device_selector) },
      m_queue_requests{ allocator }
{
}

auto DeviceBuilder::get_allocator() const -> allocator_type
{
    return m_queue_requests.get_allocator();
}

auto create_device_queue_create_infos(
    [[kiln_lifetimebound]] const std::vector<vulkan::QueueFamilyInfo>& family_infos
) -> std::vector<vk::DeviceQueueCreateInfo>
{
    std::vector<vk::DeviceQueueCreateInfo> result;
    result.reserve(family_infos.size());

    for (const vulkan::QueueFamilyInfo& family_info : family_infos)
    {
        result.push_back(static_cast<const vk::DeviceQueueCreateInfo&>(family_info));
    }

    return result;
}

auto DeviceBuilder::build(const vulkan::Instance& instance) const -> Device
{
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
        m_optional_capabilities.features_chain()
    };
    vulkan::StructureChain<vk::PhysicalDeviceFeatures2> optional_feature_support{
        m_optional_capabilities.features_chain()
    };
    physical_device.getDispatcher()->vkGetPhysicalDeviceFeatures2(
        *physical_device, optional_feature_support.root()
    );
    supported_optional_features.erase_unsupported_features(
        optional_feature_support.root()
    );
    capabilities.insert_features(supported_optional_features);

    // TODO: use std::inplace_vector
    std::vector       queue_family_infos{ create_queue_family_infos(physical_device) };
    const std::vector queue_create_infos{
        create_device_queue_create_infos(queue_family_infos)
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
        std::move(physical_device),
        std::move(device),
        std::move(capabilities),
    };
}

auto DeviceBuilder::create_queue_family_infos(
    const vk::raii::PhysicalDevice& physical_device
) const -> std::vector<vulkan::QueueFamilyInfo>
{
    std::vector<vulkan::QueueFamilyInfo> result;

    for (const ErasedQueueRequest& queue_request : m_queue_requests)
    {
        queue_request->prepare_queue(result, physical_device);
    }

    return result;
}

}   // namespace internal

}   // namespace kiln::gfx::renderer
