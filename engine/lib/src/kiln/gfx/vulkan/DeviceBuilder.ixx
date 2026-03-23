module;

#include <algorithm>
#include <cstdint>
#include <memory_resource>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

export module kiln.gfx.vulkan.DeviceBuilder;

import vulkan_hpp;

import kiln.gfx.vulkan.Device;
import kiln.gfx.vulkan.ErasedQueueRequest;
import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.PhysicalDeviceFilter;
import kiln.gfx.vulkan.structure_chain.feature_struct_c;
import kiln.gfx.vulkan.QueueFamilyInfo;
import kiln.util.StringLiteral;

namespace kiln::gfx::vulkan {

export class DeviceBuilder {
public:
    // required for interfacing with the standard
    using allocator_type =   // NOLINT(*-identifier-naming)
        std::pmr::polymorphic_allocator<>;


    DeviceBuilder() = default;
    explicit DeviceBuilder(const allocator_type& allocator);
    DeviceBuilder(const DeviceBuilder&, const allocator_type& allocator);
    DeviceBuilder(DeviceBuilder&&, const allocator_type& allocator);

    explicit DeviceBuilder(PhysicalDeviceFilter&& physical_device_selector);
    explicit DeviceBuilder(
        std::allocator_arg_t,
        const allocator_type&  allocator,
        PhysicalDeviceFilter&& physical_device_selector
    );


    // required for interfacing with the standard
    [[nodiscard]]
    auto get_allocator() const -> allocator_type;


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

    template <typename Self_T, typename QueueRequest_T>
        requires(ErasedQueueRequest::storable<QueueRequest_T>())
    auto request_queue(this Self_T&&, QueueRequest_T&& queue_request) -> Self_T&&;

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
    PhysicalDeviceFilter                 m_physical_device_filter;
    PhysicalDeviceCapabilities           m_optional_capabilities;
    std::pmr::vector<ErasedQueueRequest> m_queue_requests;

    [[nodiscard]]
    auto create_queue_family_infos(const vk::raii::PhysicalDevice& physical_device) const
        -> std::vector<QueueFamilyInfo>;
};

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan {

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

template <typename Self_T, feature_struct_c FeaturesStruct_T>
auto DeviceBuilder::enable_features(this Self_T&& self, const FeaturesStruct_T& features)
    -> Self_T&&
{
    self.m_optional_capabilities.erase_features(features);
    self.m_physical_device_filter.require_features(features);
    return std::forward<Self_T>(self);
}

template <typename Self_T, feature_struct_c FeaturesStruct_T>
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

template <typename Self_T, typename QueueRequest_T>
    requires(ErasedQueueRequest::storable<QueueRequest_T>())
auto DeviceBuilder::request_queue(this Self_T&& self, QueueRequest_T&& queue_request)
    -> Self_T&&
{
    self.DeviceBuilder::m_physical_device_filter.add_custom_requirement(
        [queue_request](const vk::raii::PhysicalDevice& physical_device) -> bool
        {
            return queue_request.is_suitable(physical_device);   //
        }
    );
    self.DeviceBuilder::m_queue_requests.emplace_back(std::move(queue_request));
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto DeviceBuilder::require_and_enable_capabilities(
    this Self_T&&                     self,
    const PhysicalDeviceCapabilities& capabilities
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

}   // namespace kiln::gfx::vulkan
