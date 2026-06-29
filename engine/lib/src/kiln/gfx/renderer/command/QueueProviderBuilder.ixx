module;

#include <memory_resource>
#include <vector>

export module kiln.gfx.renderer.command.QueueProviderBuilder;

import vulkan;

import kiln.app.registry.BuildableEntryBuilder;
import kiln.app.registry.BuildDirector;
import kiln.gfx.renderer.command.QueueProvider;
import kiln.gfx.renderer.command.QueueType;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.PhysicalDeviceFilter;
import kiln.gfx.vulkan.QueueFamilyInfo;
import kiln.util.EnumMask;
import kiln.wsi.Context;

namespace kiln::gfx::renderer {

export class QueueProviderBuilder;

auto describe_build(app::BuildDirector<QueueProviderBuilder>& build_director) -> void;

export class QueueProviderBuilder
    : public app::BuildableEntryBuilder<QueueProviderBuilder, describe_build> {
public:
    auto require_queue(QueueType type) -> void;
    auto request_queue(QueueType type) -> void;

    [[nodiscard]]
    auto device_requirement() const -> vulkan::PhysicalDeviceFilter::CustomRequirement;

    [[nodiscard]]
    auto create_queue_family_infos(
        const vulkan::Instance&                  instance,
        const wsi::Context&                      wsi_context,
        const vk::raii::PhysicalDevice&          physical_device,
        const std::pmr::polymorphic_allocator<>& allocator
    ) const -> std::pmr::vector<vulkan::QueueFamilyInfo>;

    [[nodiscard]]
    auto build(const Device& device) const -> QueueProvider;

private:
    util::EnumMask<QueueType> m_required_queue_types;
    util::EnumMask<QueueType> m_requested_queue_types;
};

}   // namespace kiln::gfx::renderer
