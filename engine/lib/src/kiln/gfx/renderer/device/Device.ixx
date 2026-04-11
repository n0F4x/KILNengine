module;

#include <algorithm>
#include <cstdint>
#include <memory_resource>
#include <optional>
#include <string>
#include <utility>

export module kiln.gfx.renderer.device.Device;

import vulkan_hpp;

import kiln.app.context.ContextBuilderInterface;
import kiln.app.memory.Arena;
import kiln.app.memory.ArenaBuilder;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.PhysicalDeviceFilter;
import kiln.gfx.vulkan.QueueFamilyInfo;
import kiln.gfx.vulkan.QueueInfo;
import kiln.gfx.vulkan.structure_chain.feature_struct_c;
import kiln.util.containers.OptionalRef;
import kiln.util.StringLiteral;
import kiln.wsi.Context;

namespace kiln::gfx::renderer {

struct QueueInfos {
    std::optional<vulkan::QueueInfo> graphics_queue_info;
    std::optional<vulkan::QueueInfo> host_to_device_transfer_queue_info;
};

export class Device {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;
    class Builder;
    using QueueInfos = QueueInfos;


    Device(Device&&, const allocator_type& allocator);

    explicit Device(
        vk::raii::PhysicalDevice&&           physical_device,
        vk::raii::Device&&                   logical_device,
        vulkan::PhysicalDeviceCapabilities&& capabilities,
        const QueueInfos&                    queue_infos
    );
    explicit Device(
        std::allocator_arg_t,
        const allocator_type&                allocator,
        vk::raii::PhysicalDevice&&           physical_device,
        vk::raii::Device&&                   logical_device,
        vulkan::PhysicalDeviceCapabilities&& capabilities,
        const QueueInfos&                    queue_infos
    );


    [[nodiscard]]
    auto name() const -> std::string;
    [[nodiscard]]
    auto physical_device() const noexcept -> const vk::raii::PhysicalDevice&;
    [[nodiscard]]
    auto logical_device() const noexcept -> const vk::raii::Device&;
    [[nodiscard]]
    auto capabilities() const noexcept -> const vulkan::PhysicalDeviceCapabilities&;
    [[nodiscard]]
    auto graphics_queue_info() const noexcept
        -> util::OptionalRef<const vulkan::QueueInfo>;
    [[nodiscard]]
    auto host_to_device_transfer_queue_info() const noexcept
        -> util::OptionalRef<const vulkan::QueueInfo>;

private:
    vk::raii::PhysicalDevice           m_physical_device;
    vk::raii::Device                   m_logical_device;
    vulkan::PhysicalDeviceCapabilities m_capabilities;
    QueueInfos                         m_queue_infos;
};

struct QueueRequests {
    bool graphics_queue_requested{};
    bool host_to_device_transfer_queue_requested{};
};

class Device::Builder : public app::ContextBuilderInterface {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    [[nodiscard]]
    static auto create(app::ArenaBuilder& arena_builder) -> Builder;


    Builder(const Builder&, const allocator_type& allocator);
    Builder(Builder&&, const allocator_type& allocator);

    explicit Builder() = default;
    explicit Builder(const allocator_type& allocator);
    explicit Builder(vulkan::PhysicalDeviceFilter&& physical_device_selector);
    explicit Builder(
        std::allocator_arg_t,
        const allocator_type&          allocator,
        vulkan::PhysicalDeviceFilter&& physical_device_selector
    );


    [[nodiscard]]
    auto get_allocator() const -> allocator_type;


    template <typename Self_T>
    auto require_minimum_version(this Self_T&&, uint32_t version) -> Self_T&&;

    template <typename Self_T>
    auto enable_extension(this Self_T&&, util::StringLiteral extension_name) -> Self_T&&;
    template <typename Self_T>
    auto enable_extension_if_available(this Self_T&&, util::StringLiteral extension_name)
        -> Self_T&&;

    template <typename Self_T, vulkan::feature_struct_c FeaturesStruct_T>
    auto enable_features(this Self_T&&, const FeaturesStruct_T& features) -> Self_T&&;
    template <typename Self_T, vulkan::feature_struct_c FeaturesStruct_T>
    auto enable_features_if_available(this Self_T&&, const FeaturesStruct_T& features)
        -> Self_T&&;

    template <typename Self_T>
    auto require_and_enable_capabilities(
        this Self_T&&,
        const vulkan::PhysicalDeviceCapabilities& capabilities
    ) -> Self_T&&;

    template <typename Self_T, typename... Args_T>
    auto add_custom_requirement(this Self_T&&, Args_T&&... args) -> Self_T&&;

    auto request_graphics_queue() -> void;
    auto request_host_to_device_transfer_queue() -> void;

    [[nodiscard]]
    auto build(
        app::Arena&             memory_arena,
        const vulkan::Instance& instance,
        const wsi::Context&     wsi_context
    ) && -> Device;

private:
    vulkan::PhysicalDeviceFilter       m_physical_device_filter;
    vulkan::PhysicalDeviceCapabilities m_optional_capabilities;
    QueueRequests                      m_queue_requests;


    [[nodiscard]]
    auto create_queue_family_infos(
        const vulkan::Instance&                  instance,
        const wsi::Context&                      wsi_context,
        const vk::raii::PhysicalDevice&          physical_device,
        QueueInfos&                              out_queue_infos,
        const std::pmr::polymorphic_allocator<>& allocator
    ) const -> std::pmr::vector<vulkan::QueueFamilyInfo>;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

template <typename Self_T>
auto Device::Builder::require_minimum_version(this Self_T&& self, const uint32_t version)
    -> Self_T&&
{
    self.m_physical_device_filter.require_minimum_version(version);
    self.m_optional_capabilities.upgrade_version(version);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto Device::Builder::enable_extension(
    this Self_T&&             self,
    const util::StringLiteral extension_name
) -> Self_T&&
{
    self.m_optional_capabilities.erase_extension(extension_name);
    self.m_physical_device_filter.require_extension(extension_name);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto Device::Builder::enable_extension_if_available(
    this Self_T&&             self,
    const util::StringLiteral extension_name
) -> Self_T&&
{
    if (!std::ranges::contains(
            self.m_physical_device_filter.required_capabilities().extensions(),
            extension_name
        ))
    {
        self.m_optional_capabilities.insert_extension(extension_name);
    }

    return std::forward<Self_T>(self);
}

template <typename Self_T, vulkan::feature_struct_c FeaturesStruct_T>
auto Device::Builder::enable_features(this Self_T&& self, const FeaturesStruct_T& features)
    -> Self_T&&
{
    self.m_optional_capabilities.erase_features(features);
    self.m_physical_device_filter.require_features(features);
    return std::forward<Self_T>(self);
}

template <typename Self_T, vulkan::feature_struct_c FeaturesStruct_T>
auto Device::Builder::enable_features_if_available(
    this Self_T&&           self,
    const FeaturesStruct_T& features
) -> Self_T&&
{
    FeaturesStruct_T copy{ features };
    self.m_physical_device_filter.required_capabilities().filter_uncontained_features(
        copy
    );
    self.m_optional_capabilities.insert_features(copy);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto Device::Builder::require_and_enable_capabilities(
    this Self_T&&                             self,
    const vulkan::PhysicalDeviceCapabilities& capabilities
) -> Self_T&&
{
    self.m_physical_device_filter.require_capabilities(capabilities);
    self.m_optional_capabilities.upgrade_version(capabilities.version());
    self.m_optional_capabilities.erase(capabilities);
    return std::forward<Self_T>(self);
}

template <typename Self_T, typename... Args_T>
auto Device::Builder::add_custom_requirement(this Self_T&& self, Args_T&&... args)
    -> Self_T&&
{
    self.m_physical_device_filter.add_custom_requirement(std::forward<Args_T>(args)...);
    return std::forward<Self_T>(self);
}

}   // namespace kiln::gfx::renderer
