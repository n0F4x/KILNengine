module;

#include <algorithm>
#include <cstdint>
#include <optional>
#include <ranges>
#include <tuple>
#include <utility>
#include <vector>

export module kiln.gfx.vulkan.DeviceBuilder;

import vulkan_hpp;

import kiln.gfx.vulkan.Device;
import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.PhysicalDeviceSelector;
import kiln.gfx.vulkan.structure_chain.feature_struct_c;
import kiln.gfx.vulkan.queue_properties;
import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.QueueGroup;
import kiln.util.containers.Bool;
import kiln.util.containers.CopyableFunction;
import kiln.util.StringLiteral;

namespace kiln::gfx::vulkan {

export class DeviceBuilder {
public:
    using QueueRequirement = util::CopyableFunction<bool(
        const vk::raii::PhysicalDevice&,
        QueueFamilyIndex,
        const vk::QueueFamilyProperties2&
    ) const>;

    explicit DeviceBuilder(PhysicalDeviceSelector&& physical_device_selector = {});

    template <typename Self_T>
    auto require_minimum_version(this Self_T&&, uint32_t version) -> Self_T&&;

    template <typename Self_T>
    auto enable_extension(this Self_T&&, util::StringLiteral extension_name) -> Self_T&&;
    template <typename Self_T>
    auto enable_extension_if_available(this Self_T&&, util::StringLiteral extension_name)
        -> Self_T&&;

    template <typename Self_T, feature_struct_c FeaturesStruct_T>
    auto enable_features(this Self_T&&, const FeaturesStruct_T& features) -> Self_T&&;
    template <typename Self_T, feature_struct_c FeaturesStruct_T>
    auto enable_features_if_available(this Self_T&&, const FeaturesStruct_T& features)
        -> Self_T&&;

    template <typename Self_T>
    auto request_graphics_queue(this Self_T&&) -> Self_T&&;
    template <typename Self_T>
    auto request_compute_queue(this Self_T&&) -> Self_T&&;
    template <typename Self_T>
    auto request_host_to_device_transfer_queue(this Self_T&&) -> Self_T&&;
    template <typename Self_T>
    auto request_device_to_host_transfer_queue(this Self_T&&) -> Self_T&&;
    template <typename Self_T>
    auto request_dedicated_sparse_binding_queue(this Self_T&&) -> Self_T&&;
    template <typename Self_T, typename... Args_T>
    auto ensure_queue(this Self_T&&, Args_T&&... args) -> Self_T&&
        requires std::constructible_from<QueueRequirement, Args_T&&...>;

    template <typename Self_T>
    auto require_and_enable_capabilities(
        this Self_T&&,
        const PhysicalDeviceCapabilities& capabilities
    ) -> Self_T&&;

    template <typename Self_T, typename... Args_T>
    auto add_custom_requirement(this Self_T&&, Args_T&&... args) -> Self_T&&;

    [[nodiscard]]
    auto build(const vk::raii::Instance& instance) const -> std::optional<Device>;

private:
    [[nodiscard]]
    static auto make_queue_group(
        const vk::raii::Device&  device,
        QueueGroup::CreateInfo&& create_info
    ) -> QueueGroup;

    PhysicalDeviceSelector        m_physical_device_selector;
    PhysicalDeviceCapabilities    m_optional_capabilities;
    util::Bool                    m_request_graphics_queue;
    util::Bool                    m_request_compute_queue;
    bool                          m_request_host_to_device_transfer_queue{};
    bool                          m_request_device_to_host_transfer_queue{};
    std::vector<QueueRequirement> m_extra_queue_requirements;
    bool                          m_request_dedicated_sparse_binding_queue{};

    [[nodiscard]]
    auto most_suitable(std::vector<vk::raii::PhysicalDevice>&& physical_devices) const
        -> std::optional<vk::raii::PhysicalDevice>;

    [[nodiscard]]
    auto create_device_queue_create_infos(
        const vk::raii::PhysicalDevice& physical_device
    ) const
        -> std::tuple<
            std::vector<std::vector<float>>,
            std::vector<vk::DeviceQueueCreateInfo>,
            QueueGroup::CreateInfo>;
};

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan {

template <typename Self_T>
auto DeviceBuilder::require_minimum_version(this Self_T&& self, const uint32_t version)
    -> Self_T&&
{
    self.m_physical_device_selector.require_minimum_version(version);
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
    self.m_physical_device_selector.require_extension(extension_name);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::enable_extension_if_available(
    this Self_T&&             self,
    const util::StringLiteral extension_name
) -> Self_T&&
{
    if (!std::ranges::contains(
            self.m_physical_device_selector.required_capabilities().extensions(),
            extension_name
        ))
    {
        self.m_optional_capabilities.insert_extension(extension_name);
    }

    return std::forward<Self_T>(self);
}

template <typename Self_T, feature_struct_c FeaturesStruct_T>
auto DeviceBuilder::enable_features(this Self_T&& self, const FeaturesStruct_T& features)
    -> Self_T&&
{
    self.m_optional_capabilities.erase_features(features);
    self.m_physical_device_selector.require_features(features);
    return std::forward<Self_T>(self);
}

template <typename Self_T, feature_struct_c FeaturesStruct_T>
auto DeviceBuilder::enable_features_if_available(
    this Self_T&&           self,
    const FeaturesStruct_T& features
) -> Self_T&&
{
    FeaturesStruct_T copy{ features };
    self.m_physical_device_selector.required_capabilities().filter_uncontained_features(
        copy
    );
    self.m_optional_capabilities.insert_features(copy);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::request_graphics_queue(this Self_T&& self) -> Self_T&&
{
    self.m_physical_device_selector.require_queue_flag(vk::QueueFlagBits::eGraphics);
    self.m_request_graphics_queue = true;
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::request_compute_queue(this Self_T&& self) -> Self_T&&
{
    self.m_physical_device_selector.require_queue_flag(vk::QueueFlagBits::eCompute);
    self.m_request_compute_queue = true;
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::request_host_to_device_transfer_queue(this Self_T&& self) -> Self_T&&
{
    self.m_physical_device_selector.require_queue_flag(vk::QueueFlagBits::eTransfer);
    self.m_request_host_to_device_transfer_queue = true;
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::request_device_to_host_transfer_queue(this Self_T&& self) -> Self_T&&
{
    self.m_physical_device_selector.require_queue_flag(vk::QueueFlagBits::eTransfer);
    self.m_request_device_to_host_transfer_queue = true;
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::request_dedicated_sparse_binding_queue(this Self_T&& self) -> Self_T&&
{
    self.m_request_dedicated_sparse_binding_queue = true;
    return std::forward<Self_T>(self);
}

template <typename Self_T, typename... Args_T>
auto DeviceBuilder::ensure_queue(this Self_T&& self, Args_T&&... args) -> Self_T&&
    requires std::constructible_from<QueueRequirement, Args_T&&...>
{
    const QueueRequirement& queue_requirement =
        self.m_extra_queue_requirements.emplace_back(std::forward<Args_T>(args)...);

    self.add_custom_requirement(
        [queue_requirement](const vk::raii::PhysicalDevice& physical_device) -> bool
        { return has_matching_queue_family_index(physical_device, queue_requirement); }
    );

    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::require_and_enable_capabilities(
    this Self_T&&                     self,
    const PhysicalDeviceCapabilities& capabilities
) -> Self_T&&
{
    self.m_physical_device_selector.require_capabilities(capabilities);
    self.m_optional_capabilities.upgrade_version(capabilities.version());
    self.m_optional_capabilities.erase(capabilities);
    return std::forward<Self_T>(self);
}

template <typename Self_T, typename... Args_T>
auto DeviceBuilder::add_custom_requirement(this Self_T&& self, Args_T&&... args)
    -> Self_T&&
{
    self.m_physical_device_selector.add_custom_requirement(std::forward<Args_T>(args)...);
    return std::forward<Self_T>(self);
}

}   // namespace kiln::gfx::vulkan
