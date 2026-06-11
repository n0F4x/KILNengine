module;

#include <memory_resource>
#include <optional>
#include <string>
#include <vector>

export module kiln.gfx.renderer.device.Device;

import vulkan_hpp;

import kiln.app.registry.BuildableEntry;
import kiln.app.registry.EntryBuildDirector;
import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.QueueFamilyInfo;
import kiln.gfx.vulkan.QueueInfo;
import kiln.util.containers.OptionalRef;

namespace kiln::gfx::renderer {

struct QueueInfos {
    std::optional<vulkan::QueueInfo> graphics_queue_info;
    std::optional<vulkan::QueueInfo> compute_queue_info;
    std::optional<vulkan::QueueInfo> host_to_device_transfer_queue_info;
};

export class Device;

auto describe_build(app::EntryBuildDirector<Device>& build_director) -> void;

class Device : public app::BuildableEntry<Device, describe_build> {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;
    using QueueInfos = QueueInfos;


    Device(Device&&, const allocator_type& allocator);

    explicit Device(
        vk::raii::PhysicalDevice&&                  physical_device,
        vk::raii::Device&&                          logical_device,
        vulkan::PhysicalDeviceCapabilities&&        capabilities,
        std::pmr::vector<vulkan::QueueFamilyInfo>&& queue_family_infos,
        const QueueInfos&                           queue_infos
    );
    explicit Device(
        std::allocator_arg_t,
        const allocator_type&                       allocator,
        vk::raii::PhysicalDevice&&                  physical_device,
        vk::raii::Device&&                          logical_device,
        vulkan::PhysicalDeviceCapabilities&&        capabilities,
        std::pmr::vector<vulkan::QueueFamilyInfo>&& queue_family_infos,
        const QueueInfos&                           queue_infos
    );


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto name() const -> std::string;
    [[nodiscard]]
    auto physical_device() const noexcept -> const vk::raii::PhysicalDevice&;
    [[nodiscard]]
    auto logical_device() const noexcept -> const vk::raii::Device&;
    [[nodiscard]]
    auto capabilities() const noexcept -> const vulkan::PhysicalDeviceCapabilities&;
    [[nodiscard]]
    auto queue_family(vulkan::QueueFamilyIndex family_index) const noexcept
        -> const vulkan::QueueFamilyInfo&;
    [[nodiscard]]
    auto graphics_queue_info() const noexcept
        -> util::OptionalRef<const vulkan::QueueInfo>;
    [[nodiscard]]
    auto compute_queue_info() const noexcept
        -> util::OptionalRef<const vulkan::QueueInfo>;
    [[nodiscard]]
    auto host_to_device_transfer_queue_info() const noexcept
        -> util::OptionalRef<const vulkan::QueueInfo>;

private:
    vk::raii::PhysicalDevice                  m_physical_device;
    vk::raii::Device                          m_logical_device;
    vulkan::PhysicalDeviceCapabilities        m_capabilities;
    std::pmr::vector<vulkan::QueueFamilyInfo> m_queue_family_infos;
    QueueInfos                                m_queue_infos;
};

}   // namespace kiln::gfx::renderer
