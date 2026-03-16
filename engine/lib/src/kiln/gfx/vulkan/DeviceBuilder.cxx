module;

#include <algorithm>
#include <cassert>
#include <functional>
#include <optional>
#include <ranges>
#include <vector>

module kiln.gfx.vulkan.DeviceBuilder;

import kiln.util.Lazy;
import kiln.gfx.vulkan.result.check_result;
import kiln.gfx.vulkan.structure_chain.StructureChain;
import kiln.gfx.vulkan.QueuePack;

namespace kiln::gfx::vulkan {

DeviceBuilder::DeviceBuilder(PhysicalDeviceFilter&& physical_device_selector)
    : m_physical_device_filter{ std::move(physical_device_selector) }
{
}

auto DeviceBuilder::build(const vk::raii::Instance& instance) const
    -> std::optional<Device>
{
    std::vector<vk::raii::PhysicalDevice> supported_devices{
        check_result(instance.enumeratePhysicalDevices())
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
        return std::nullopt;
    }

    vk::raii::PhysicalDevice physical_device{
        *most_suitable(std::move(supported_devices))
    };

    PhysicalDeviceCapabilities capabilities{
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

    StructureChain<vk::PhysicalDeviceFeatures2> supported_optional_features{
        m_optional_capabilities.features_chain()
    };
    StructureChain<vk::PhysicalDeviceFeatures2> optional_feature_support{
        m_optional_capabilities.features_chain()
    };
    physical_device.getDispatcher()->vkGetPhysicalDeviceFeatures2(
        *physical_device, optional_feature_support.root()
    );
    supported_optional_features.erase_unsupported_features(
        optional_feature_support.root()
    );
    capabilities.insert_features(supported_optional_features);

    auto [per_family_queue_priorities, queue_create_infos, queue_group_create_info]{
        create_device_queue_create_infos(physical_device)
    };
    for (auto&& [priorities, queue_create_info] :
         std::views::zip(std::as_const(per_family_queue_priorities), queue_create_infos))
    {
        queue_create_info.pQueuePriorities = priorities.data();
    }

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
        check_result(physical_device.createDevice(device_create_info))
    };

    QueueGroup queue_group{ make_queue_group(device, std::move(queue_group_create_info)) };

    return Device{
        .physical_device{ std::move(physical_device) },
        .logical_device{ std::move(device) },
        .queues{ std::move(queue_group) },
        .enabled_capabilities{ std::move(capabilities) },
    };
}

auto DeviceBuilder::make_queue_group(
    const vk::raii::Device&  device,
    QueueGroup::CreateInfo&& create_info
) -> QueueGroup
{
    for (auto& [family_index, queue_index, queue] : create_info.queue_packs)
    {
        const vk::DeviceQueueInfo2 queue_info{
            .queueFamilyIndex = family_index.underlying(),
            .queueIndex       = queue_index,
        };
        queue = check_result(device.getQueue2(queue_info));
    }

    return QueueGroup(std::move(create_info));
}

auto DeviceBuilder::most_suitable(
    std::vector<vk::raii::PhysicalDevice>&& physical_devices
) const -> std::optional<vk::raii::PhysicalDevice>
{
    if (physical_devices.empty())
    {
        return std::nullopt;
    }

    std::size_t highest_scoring_index{};
    uint32_t    highest_score{};

    // TODO: use std::views::enumerate
    for (auto&& [index, physical_device] :
         std::views::zip(std::views::iota(0uz), physical_devices))
    {
        uint32_t score{};

        if (m_request_dedicated_sparse_binding_queue
            && has_dedicated_sparse_binding_queue_family(physical_device))
        {
            score += 1 << 0;
        }
        if ((m_request_host_to_device_transfer_queue
             || m_request_device_to_host_transfer_queue)
            && has_dedicated_transfer_queue_family(physical_device))
        {
            score += 1 << 1;
        }
        if (m_request_compute_queue && has_dedicated_compute_queue_family(physical_device))
        {
            score += 1 << 2;
        }

        if (score > highest_score)
        {
            highest_scoring_index = index;
            highest_score         = score;
        }
    }

    return std::move(physical_devices[highest_scoring_index]);
}

auto DeviceBuilder::create_device_queue_create_infos(
    const vk::raii::PhysicalDevice& physical_device
) const
    -> std::tuple<
        std::vector<std::vector<float>>,
        std::vector<vk::DeviceQueueCreateInfo>,
        QueueGroup::CreateInfo>
{
    std::tuple<
        std::vector<std::vector<float>>,
        std::vector<vk::DeviceQueueCreateInfo>,
        QueueGroup::CreateInfo>
                                     result{};
    std::vector<std::vector<float>>& per_family_queue_priorities{
        std::get<std::vector<std::vector<float>>>(result)
    };
    std::vector<vk::DeviceQueueCreateInfo>& device_queue_create_infos{
        std::get<std::vector<vk::DeviceQueueCreateInfo>>(result)
    };
    QueueGroup::CreateInfo& queue_group_create_info{
        std::get<QueueGroup::CreateInfo>(result)
    };

    const std::vector<vk::QueueFamilyProperties2> queue_family_properties{
        physical_device.getQueueFamilyProperties2()
    };

    const auto device_queue_create_info =                            //
        [&per_family_queue_priorities, &device_queue_create_infos]   //
        (
            const QueueFamilyIndex family_index
        ) -> std::pair<vk::DeviceQueueCreateInfo&, std::vector<float>&>   //
    {
        const auto iter = std::ranges::find_if(
            device_queue_create_infos,   //
            [family_index](const vk::DeviceQueueCreateInfo& create_info) -> bool
            {
                return create_info.queueFamilyIndex == family_index.underlying();   //
            }
        );
        if (iter != device_queue_create_infos.cend())
        {
            return std::pair<vk::DeviceQueueCreateInfo&, std::vector<float>&>{
                *iter,
                per_family_queue_priorities[static_cast<uint32_t>(
                    std::distance(device_queue_create_infos.begin(), iter)
                )]
            };
        }

        std::pair<vk::DeviceQueueCreateInfo&, std::vector<float>&> x_result{
            device_queue_create_infos.emplace_back(),
            per_family_queue_priorities.emplace_back()
        };
        x_result.first.queueFamilyIndex = family_index.underlying();
        return x_result;
    };

    const auto queue_count_can_be_incremented =
        [&device_queue_create_info, &queue_family_properties]   //
        [[nodiscard]]
        (const QueueFamilyIndex family_index) -> bool           //
    {
        return device_queue_create_info(family_index).first.queueCount
             < queue_family_properties[family_index.underlying()]
                   .queueFamilyProperties.queueCount;
    };

    const auto try_setup_new_queue =   //
        [&device_queue_create_info,
         &queue_family_properties,
         &queue_group_create_info]                          //
        (const QueueFamilyIndex family_index,
         const float            priority) -> std::optional<uint32_t>   //
    {
        const auto& [queue_create_info, queue_priorities]{
            device_queue_create_info(family_index)
        };

        if (queue_create_info.queueCount
            < queue_family_properties[family_index.underlying()]
                  .queueFamilyProperties.queueCount)
        {
            queue_priorities.push_back(priority);
            ++queue_create_info.queueCount;

            queue_group_create_info.queue_packs.push_back(
                QueuePack{
                    .family_index = family_index,
                    .queue_index  = queue_create_info.queueCount - 1,
                    .queue        = nullptr,
                }
            );

            return queue_group_create_info.queue_packs.size() - 1;
        }

        return std::nullopt;
    };

    const auto next_incrementable_family_index =   //
        [&queue_family_properties, &queue_count_can_be_incremented](
            const vk::QueueFlagBits flag
        ) -> std::optional<QueueFamilyIndex>   //
    {
        for (const uint32_t family_index :
             std::views::iota(0u, queue_family_properties.size()))
        {
            if (queue_family_properties[family_index].queueFamilyProperties.queueFlags
                    & flag
                && queue_count_can_be_incremented(QueueFamilyIndex{ family_index }))
            {
                return QueueFamilyIndex{ family_index };
            }
        }
        return std::nullopt;
    };

    const std::optional<QueueFamilyIndex> graphics_queue_family{
        m_request_graphics_queue.transform(
            [&physical_device] -> QueueFamilyIndex
            { return *first_graphics_queue_family_index(physical_device); }
        )
    };

    if (m_request_graphics_queue)
    {
        queue_group_create_info.graphics_queue_pack_index =
            *try_setup_new_queue(*graphics_queue_family, 0.5f);
    }

    const std::optional<QueueFamilyIndex> compute_queue_family{
        m_request_compute_queue.transform(
            [&physical_device,
             &next_incrementable_family_index,
             &graphics_queue_family] -> QueueFamilyIndex   //
            {
                return first_dedicated_compute_queue_family_index(physical_device)
                    .or_else(
                        std::bind_front(
                            std::cref(next_incrementable_family_index),
                            vk::QueueFlagBits::eCompute
                        )
                    )
                    .value_or(
                        util::Lazy{
                            [&graphics_queue_family] -> QueueFamilyIndex
                            { return *graphics_queue_family; }   //
                        }
                    );
            }
        )   //
    };

    if (m_request_compute_queue)
    {
        queue_group_create_info.compute_queue_pack_index =
            try_setup_new_queue(*compute_queue_family, 0.5f)
                .value_or(   //
                    util::Lazy{
                        [&queue_group_create_info] -> uint32_t
                        { return *queue_group_create_info.graphics_queue_pack_index; }   //
                    }
                );
    }

    const auto next_transfer_family = [&graphics_queue_family,
                                       &compute_queue_family,
                                       &next_incrementable_family_index](
                                          const vk::raii::PhysicalDevice& x_physical_device
                                      ) -> QueueFamilyIndex   //
    {
        return first_dedicated_transfer_queue_family_index(x_physical_device)
            .or_else(
                std::bind_front(
                    std::cref(next_incrementable_family_index),
                    vk::QueueFlagBits::eTransfer
                )
            )
            .value_or(
                util::Lazy{
                    [&graphics_queue_family,
                     &compute_queue_family] -> QueueFamilyIndex   //
                    {
                        if (graphics_queue_family.has_value())
                        {
                            return *graphics_queue_family;
                        }
                        if (compute_queue_family.has_value())
                        {
                            return *compute_queue_family;
                        }
                        std::unreachable();
                    }   //
                }
            );
    };

    const auto setup_new_transfer_queue =                        //
        [&queue_group_create_info, &try_setup_new_queue, this]   //
        (const QueueFamilyIndex family_index) -> uint32_t        //
    {
        return try_setup_new_queue(family_index, 0.2f)
            .value_or(
                util::Lazy{
                    [&queue_group_create_info, this] -> uint32_t
                    {
                        if (m_request_graphics_queue)
                        {
                            return *queue_group_create_info.graphics_queue_pack_index;
                        }
                        if (m_request_compute_queue)
                        {
                            return *queue_group_create_info.compute_queue_pack_index;
                        }
                        std::unreachable();
                    }   //
                }
            );
    };

    if (m_request_host_to_device_transfer_queue)
    {
        queue_group_create_info.host_to_device_transfer_queue_pack_index =
            setup_new_transfer_queue(next_transfer_family(physical_device));
    }

    if (m_request_device_to_host_transfer_queue)
    {
        queue_group_create_info.device_to_host_transfer_queue_pack_index =
            setup_new_transfer_queue(next_transfer_family(physical_device));
    }

    if (m_request_dedicated_sparse_binding_queue)
    {
        const std::optional<QueueFamilyIndex> dedicated_sparse_binding_family{
            first_dedicated_transfer_queue_family_index(physical_device)
        };

        if (dedicated_sparse_binding_family.has_value())
        {
            queue_group_create_info.dedicated_sparse_binding_queue_pack_index =
                try_setup_new_queue(*dedicated_sparse_binding_family, 0.7f);
        }
    }

    for (const QueueRequirement& queue_requirement : m_extra_queue_requirements)
    {
        if (std::ranges::none_of(
                queue_group_create_info.queue_packs,
                [&queue_requirement,
                 &physical_device,
                 &queue_family_properties](const QueuePack& queue_pack) -> bool
                {
                    return queue_requirement(
                        physical_device,
                        queue_pack.family_index,
                        queue_family_properties[queue_pack.family_index.underlying()]
                    );
                }
            ))
        {
            [[maybe_unused]]
            const bool success =
                try_setup_new_queue(
                    *first_matching_queue_family_index(physical_device, queue_requirement),
                    0.5f
                )
                    .has_value();
            assert(success);
        }
    }

    return result;
}

}   // namespace kiln::gfx::vulkan
