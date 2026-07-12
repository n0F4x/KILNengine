module;

#include <array>
#include <cstdint>
#include <memory_resource>
#include <optional>
#include <ranges>
#include <span>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.command.QueueProviderBuilder;

import vulkan;

import kiln.gfx.renderer.command.Queue;
import kiln.gfx.renderer.command.QueueType;
import kiln.gfx.renderer.device.DeviceBuilder;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.QueueFamilyInfo;
import kiln.gfx.vulkan.QueueInfo;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;
import kiln.util.EnumMask;
import kiln.wsi.vulkan_queue_family_supports_presenting;

namespace kiln::gfx::renderer {

[[nodiscard]]
auto make_builder(vulkan::InstanceBuilder& instance_builder, DeviceBuilder& device_builder)
    -> QueueProviderBuilder
{
    /*
     * Vulkan 1.4 requires that graphics and command queues also support transfer
     * operations
     */
    instance_builder.target_api_version(vk::ApiVersion14);
    device_builder.require_minimum_version(vk::ApiVersion14);

    device_builder.enable_features(
        vk::PhysicalDeviceVulkan13Features{ .synchronization2 = vk::True }
    );
    device_builder.enable_features(
        vk::PhysicalDeviceVulkan14Features{ .maintenance6 = vk::True }
    );

    return QueueProviderBuilder{};
}

auto describe_build(reg::BuildDirector<QueueProviderBuilder>& build_director) -> void
{
    build_director.use_function<make_builder>();
}

auto QueueProviderBuilder::require_queue(const QueueType type) -> void
{
    m_required_queue_types |= type;
    m_requested_queue_types |= type;
}

auto QueueProviderBuilder::request_queue(const QueueType type) -> void
{
    m_requested_queue_types |= type;
}

[[nodiscard]]
// ReSharper disable once CppNotAllPathsReturnValue
auto queue_flag_from(const QueueType queue_type) -> vk::QueueFlags
{
    switch (queue_type)
    {
        case QueueType::eGraphics:             return vk::QueueFlagBits::eGraphics;
        case QueueType::eCompute:              return vk::QueueFlagBits::eCompute;
        case QueueType::eHostToDeviceTransfer: return vk::QueueFlagBits::eTransfer;
    }
}

[[nodiscard]]
auto queue_flags_from(const util::EnumMask<QueueType> queue_types) -> vk::QueueFlags
{
    vk::QueueFlags result;

    for (const QueueType queue_type : magic_enum::enum_values<QueueType>())
    {
        if (queue_types & queue_type)
        {
            result |= queue_flag_from(queue_type);
        }
    }

    return result;
}

auto QueueProviderBuilder::device_requirement() const
    -> vulkan::PhysicalDeviceFilter::CustomRequirement
{
    return vulkan::PhysicalDeviceFilter::CustomRequirement{
        [required_queue_types = m_required_queue_types](
            const vk::raii::PhysicalDevice& physical_device
        ) -> bool
        {
            vk::QueueFlags remaining_queue_types{ queue_flags_from(required_queue_types) };

            const std::vector<vk::QueueFamilyProperties2> queue_families{
                physical_device.getQueueFamilyProperties2()
            };

            for (const vk::QueueFamilyProperties2 family_properties : queue_families)
            {
                remaining_queue_types
                    = remaining_queue_types
                    & ~family_properties.queueFamilyProperties.queueFlags;
                if (!remaining_queue_types)
                {
                    return true;
                }
            }
            return false;
        },
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

auto QueueProviderBuilder::create_queue_family_infos(
    const vulkan::Instance&                  instance,
    const wsi::Context&                      wsi_context,
    const vk::raii::PhysicalDevice&          physical_device,
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
        try_emplace_graphics_queue(result);
    }
    if (m_requested_queue_types & QueueType::eCompute)
    {
        try_emplace_compute_queue(result);
    }
    if (m_requested_queue_types & QueueType::eHostToDeviceTransfer)
    {
        try_emplace_transfer_queue(result);
    }

    return result;
}

struct QueueInfos {
    std::optional<vulkan::QueueInfo> graphics_queue_info;
    std::optional<vulkan::QueueInfo> compute_queue_info;
    std::optional<vulkan::QueueInfo> host_to_device_transfer_queue_info;
};

[[nodiscard]]
auto find_queue_slot_index(
    const std::array<std::optional<Queue>, 3>& queue_slots,
    const vulkan::QueueInfo&                   queue_info
) -> std::optional<uint32_t>
{
    for (uint32_t index{}; index < queue_slots.size(); ++index)
    {
        if (!queue_slots[index].has_value())
        {
            return std::nullopt;
        }

        if (const Queue& queue{ *queue_slots[index] };
            queue.family_index() == queue_info.family_index
            && queue.index() == queue_info.index)
        {
            return index;
        }
    }

    return std::nullopt;
}

[[nodiscard]]
auto next_empty_queue_slot_index(const std::array<std::optional<Queue>, 3>& queue_slots)
    -> uint32_t
{
    for (uint32_t index{}; index < queue_slots.size(); ++index)
    {
        if (!queue_slots[index].has_value())
        {
            return index;
        }
    }
    std::unreachable();
}

[[nodiscard]]
auto choose_queue(
    const std::span<const vulkan::QueueFamilyInfo::DeviceCreateInfo> create_infos,
    const vk::DeviceQueueCreateFlags                                 create_flags,
    const uint32_t                                                   start_index = 0
) -> std::optional<vulkan::QueueInfo>
{
    uint32_t queue_index{ 0 };
    for (const vulkan::QueueFamilyInfo::DeviceCreateInfo& create_info : create_infos)
    {
        if (queue_index + create_info.queue_count() < start_index)
        {
            queue_index += create_info.queue_count();
            continue;
        }

        if (create_info.flags() == create_flags)
        {
            if (queue_index < start_index)
            {
                queue_index = start_index;
            }

            return vulkan::QueueInfo{
                .family_index = create_info.family_index(),
                .flags        = create_flags,
                .index        = queue_index,
            };
        }

        queue_index += create_info.queue_count();
    }

    return std::nullopt;
}

auto choose_graphics_queue(
    QueueInfos&                                    out,
    const std::span<const vulkan::QueueFamilyInfo> queue_family_infos
) -> void
{
    PRECOND(!out.graphics_queue_info.has_value());
    PRECOND(!out.compute_queue_info.has_value());
    PRECOND(!out.host_to_device_transfer_queue_info.has_value());

    for (const vulkan::QueueFamilyInfo& queue_family_info : queue_family_infos)
    {
        if (queue_family_info.flags() & vk::QueueFlagBits::eGraphics
            // TODO: move presentation support logic elsewhere
            && queue_family_info.supports_presentation())
        {
            constexpr static vk::DeviceQueueCreateFlags create_flags{};

            out.graphics_queue_info
                = choose_queue(queue_family_info.create_infos(), create_flags);
            if (out.graphics_queue_info.has_value())
            {
                return;
            }
        }
    }
}

auto choose_compute_queue(
    QueueInfos&                                    out,
    const std::span<const vulkan::QueueFamilyInfo> queue_family_infos
) -> void
{
    PRECOND(!out.compute_queue_info.has_value());
    PRECOND(!out.host_to_device_transfer_queue_info.has_value());

    constexpr static vk::DeviceQueueCreateFlags create_flags{};

    for (const vulkan::QueueFamilyInfo& queue_family_info : queue_family_infos)
    {
        if (is_dedicated_compute_queue_family(queue_family_info.flags()))
        {
            out.compute_queue_info
                = choose_queue(queue_family_info.create_infos(), create_flags);
            if (out.compute_queue_info.has_value())
            {
                return;
            }
        }
    }

    for (const vulkan::QueueFamilyInfo& queue_family_info : queue_family_infos)
    {
        if (queue_family_info.flags() & vk::QueueFlagBits::eCompute)
        {
            out.compute_queue_info
                = choose_queue(queue_family_info.create_infos(), create_flags);
            if (out.compute_queue_info.has_value()
                && out.graphics_queue_info.has_value()
                && out.graphics_queue_info->family_index == queue_family_info.index()
                && out.compute_queue_info->index == out.graphics_queue_info->index)
            {
                out.compute_queue_info = choose_queue(
                    queue_family_info.create_infos(),
                    create_flags,
                    out.graphics_queue_info->index + 1
                );
                if (out.compute_queue_info.has_value())
                {
                    return;
                }
            }
        }
    }

    if (out.graphics_queue_info.has_value()
        && (queue_family_infos[out.graphics_queue_info->family_index.underlying()].flags()
            & vk::QueueFlagBits::eCompute))
    {
        out.compute_queue_info = out.graphics_queue_info;
    }
}

auto choose_host_to_device_transfer_queue(
    QueueInfos&                                    out,
    const std::span<const vulkan::QueueFamilyInfo> queue_family_infos
) -> void
{
    PRECOND(!out.host_to_device_transfer_queue_info.has_value());

    constexpr static vk::DeviceQueueCreateFlags create_flags{};

    for (const vulkan::QueueFamilyInfo& queue_family_info : queue_family_infos)
    {
        if (is_dedicated_transfer_queue_family(queue_family_info.flags()))
        {
            out.host_to_device_transfer_queue_info
                = choose_queue(queue_family_info.create_infos(), create_flags);
            if (out.host_to_device_transfer_queue_info.has_value())
            {
                return;
            }
        }
    }

    for (const vulkan::QueueFamilyInfo& queue_family_info : queue_family_infos)
    {
        if (queue_family_info.flags() & vk::QueueFlagBits::eTransfer)
        {
            out.host_to_device_transfer_queue_info
                = choose_queue(queue_family_info.create_infos(), create_flags);
            if (out.host_to_device_transfer_queue_info.has_value())
            {
                if (out.graphics_queue_info.has_value()
                    && out.graphics_queue_info->family_index == queue_family_info.index()
                    && out.host_to_device_transfer_queue_info->index
                           == out.graphics_queue_info->index)
                {
                    out.host_to_device_transfer_queue_info = choose_queue(
                        queue_family_info.create_infos(),
                        create_flags,
                        out.graphics_queue_info->index + 1
                    );
                }

                if (out.compute_queue_info.has_value()
                    && out.compute_queue_info->family_index == queue_family_info.index()
                    && out.host_to_device_transfer_queue_info->index
                           == out.compute_queue_info->index)
                {
                    out.host_to_device_transfer_queue_info = choose_queue(
                        queue_family_info.create_infos(),
                        create_flags,
                        out.compute_queue_info->index + 1
                    );
                }
            }
        }
    }

    if (out.host_to_device_transfer_queue_info.has_value())
    {
        return;
    }

    if (out.compute_queue_info.has_value()
        && (queue_family_infos[out.compute_queue_info->family_index.underlying()].flags()
            & vk::QueueFlagBits::eCompute))
    {
        out.host_to_device_transfer_queue_info = out.compute_queue_info;
        return;
    }

    if (out.graphics_queue_info.has_value()
        && (queue_family_infos[out.graphics_queue_info->family_index.underlying()].flags()
            & vk::QueueFlagBits::eCompute))
    {
        out.host_to_device_transfer_queue_info = out.graphics_queue_info;
    }
}

[[nodiscard]]
auto make_queue_infos(
    const util::EnumMask<QueueType>                requested_queue_types,
    const std::span<const vulkan::QueueFamilyInfo> queue_family_infos
) -> QueueInfos
{
    QueueInfos result;

    if (requested_queue_types & QueueType::eGraphics)
    {
        choose_graphics_queue(result, queue_family_infos);
    }
    if (requested_queue_types & QueueType::eCompute)
    {
        choose_compute_queue(result, queue_family_infos);
    }
    if (requested_queue_types & QueueType::eHostToDeviceTransfer)
    {
        choose_host_to_device_transfer_queue(result, queue_family_infos);
    }

    return result;
}

auto QueueProviderBuilder::build(const Device& device) const -> QueueProvider
{
    std::array<std::optional<Queue>, 3> queue_slots{};
    QueueProvider::QueueIndices         queue_indices;

    const auto assign{
        [&device, &queue_slots](
            const std::optional<vulkan::QueueInfo>& queue_info,
            std::optional<uint32_t>&                queue_index
        ) -> void
        {
            if (queue_info.has_value())
            {
                std::optional<uint32_t> queue_slot_index{
                    find_queue_slot_index(queue_slots, *queue_info)
                };
                if (!queue_slot_index.has_value())
                {
                    queue_slot_index = next_empty_queue_slot_index(queue_slots);
                    queue_slots[*queue_slot_index].emplace(
                        device,
                        queue_info->family_index,
                        device.queue_family(queue_info->family_index).flags(),
                        queue_info->index
                    );
                }
                queue_index = *queue_slot_index;
            }
        },
    };

    const auto [graphics_queue_info, compute_queue_info, host_to_device_transfer_queue_info]{
        make_queue_infos(m_requested_queue_types, device.queue_families())
    };

    assign(compute_queue_info, queue_indices.compute_queue_index);
    assign(graphics_queue_info, queue_indices.graphics_queue_index);
    assign(
        host_to_device_transfer_queue_info,
        queue_indices.host_to_device_transfer_queue_index
    );


    return QueueProvider{ std::move(queue_slots), queue_indices };
}

}   // namespace kiln::gfx::renderer
