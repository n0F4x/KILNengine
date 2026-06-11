module;

#include <algorithm>
#include <memory_resource>

export module kiln.gfx.renderer.device.DeviceBuilder;

import vulkan_hpp;

import kiln.app.memory.MemoryArena;
import kiln.app.registry.EntryBuilderBase;
import kiln.gfx.renderer.device.Device;
import kiln.gfx.renderer.device.QueueType;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.PhysicalDeviceFilter;
import kiln.gfx.vulkan.QueueFamilyInfo;
import kiln.gfx.vulkan.structure_chain.feature_struct_c;
import kiln.util.EnumMask;
import kiln.util.StringLiteral;
import kiln.wsi.Context;

namespace kiln::gfx::renderer {

export class DeviceBuilder : public app::EntryBuilderBase {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    [[nodiscard]]
    static auto create(app::MemoryArena& memory_arena) -> DeviceBuilder;


    DeviceBuilder(const DeviceBuilder&, const allocator_type& allocator);
    DeviceBuilder(DeviceBuilder&&, const allocator_type& allocator);

    explicit DeviceBuilder() = default;
    explicit DeviceBuilder(const allocator_type& allocator);
    explicit DeviceBuilder(vulkan::PhysicalDeviceFilter&& physical_device_selector);
    explicit DeviceBuilder(
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

    auto request_queue(QueueType type) -> void;

    [[nodiscard]]
    auto build(
        app::MemoryArena&       memory_arena,
        const vulkan::Instance& instance,
        const wsi::Context&     wsi_context
    ) const&& -> Device;

private:
    vulkan::PhysicalDeviceFilter       m_physical_device_filter;
    vulkan::PhysicalDeviceCapabilities m_optional_capabilities;
    util::EnumMask<QueueType>          m_requested_queue_types;


    [[nodiscard]]
    auto create_queue_family_infos(
        const vulkan::Instance&                  instance,
        const wsi::Context&                      wsi_context,
        const vk::raii::PhysicalDevice&          physical_device,
        Device::QueueInfos&                      out_queue_infos,
        const std::pmr::polymorphic_allocator<>& allocator
    ) const -> std::pmr::vector<vulkan::QueueFamilyInfo>;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

template <typename Self_T>
auto DeviceBuilder::require_minimum_version(this Self_T&& self, const uint32_t version)
    -> Self_T&&
{
    self.m_physical_device_filter.require_minimum_version(version);
    self.m_optional_capabilities.upgrade_version(version);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::enable_extension(
    this Self_T&&             self,
    const util::StringLiteral extension_name
) -> Self_T&&
{
    self.m_optional_capabilities.erase_extension(extension_name);
    self.m_physical_device_filter.require_extension(extension_name);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::enable_extension_if_available(
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
auto DeviceBuilder::enable_features(this Self_T&& self, const FeaturesStruct_T& features)
    -> Self_T&&
{
    self.m_optional_capabilities.erase_features(features);
    self.m_physical_device_filter.require_features(features);
    return std::forward<Self_T>(self);
}

template <typename Self_T, vulkan::feature_struct_c FeaturesStruct_T>
auto DeviceBuilder::enable_features_if_available(
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
auto DeviceBuilder::require_and_enable_capabilities(
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
auto DeviceBuilder::add_custom_requirement(this Self_T&& self, Args_T&&... args)
    -> Self_T&&
{
    self.m_physical_device_filter.add_custom_requirement(std::forward<Args_T>(args)...);
    return std::forward<Self_T>(self);
}

}   // namespace kiln::gfx::renderer
