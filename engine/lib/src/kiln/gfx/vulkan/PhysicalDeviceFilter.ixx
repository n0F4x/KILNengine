module;

#include <concepts>
#include <cstdint>
#include <memory_resource>
#include <utility>
#include <vector>

export module kiln.gfx.vulkan.PhysicalDeviceFilter;

import vulkan;

import kiln.gfx.vulkan.PhysicalDeviceCapabilities;
import kiln.gfx.vulkan.structure_chain.feature_struct_c;
import kiln.gfx.vulkan.structure_chain.StructureChain;
import kiln.util.containers.CopyableFunction;
import kiln.util.StringLiteral;

namespace kiln::gfx::vulkan {

export class PhysicalDeviceFilter {
public:
    using CustomRequirement
        = util::CopyableFunction<auto(const vk::raii::PhysicalDevice&) const->bool>;
    using allocator_type = std::pmr::polymorphic_allocator<>;


    PhysicalDeviceFilter(const PhysicalDeviceFilter&, const allocator_type& allocator);
    PhysicalDeviceFilter(PhysicalDeviceFilter&&, const allocator_type& allocator);

    explicit PhysicalDeviceFilter() = default;
    explicit PhysicalDeviceFilter(const allocator_type& allocator);


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto required_capabilities() const noexcept -> const PhysicalDeviceCapabilities&;
    [[nodiscard]]
    auto required_queue_flags() const noexcept -> vk::QueueFlags;

    template <typename Self_T>
    auto require_minimum_version(this Self_T&&, uint32_t version) -> Self_T&&;
    template <typename Self_T>
    auto require_extension(this Self_T&&, util::StringLiteral extension_name) -> Self_T&&;
    template <typename Self_T, feature_struct_c Features_T>
    auto require_features(this Self_T&&, const Features_T& features) -> Self_T&&;
    template <typename Self_T>
    auto require_queue_flag(this Self_T&&, vk::QueueFlagBits flag) -> Self_T&&;
    template <typename Self_T>
    auto require_capabilities(
        this Self_T&&,
        const PhysicalDeviceCapabilities& capabilities
    ) -> Self_T&&;
    template <typename Self_T, typename... Args_T>
    auto add_custom_requirement(this Self_T&&, Args_T&&... args) -> Self_T&&
        requires std::constructible_from<CustomRequirement, Args_T&&...>;

    [[nodiscard]]
    auto is_adequate(const vk::raii::PhysicalDevice& physical_device) const -> bool;

private:
    PhysicalDeviceCapabilities          m_required_capabilities;
    vk::QueueFlags                      m_queue_flags;
    std::pmr::vector<CustomRequirement> m_custom_requirements;

    [[nodiscard]]
    auto supports_queues_flags(const vk::raii::PhysicalDevice& physical_device) const
        -> bool;
    [[nodiscard]]
    auto supports_custom_requirements(
        const vk::raii::PhysicalDevice& physical_device
    ) const -> bool;
};

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan {

template <typename Self_T>
auto PhysicalDeviceFilter::require_minimum_version(
    this Self_T&&  self,
    const uint32_t version
) -> Self_T&&
{
    self.m_required_capabilities.upgrade_version(version);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto PhysicalDeviceFilter::require_extension(
    this Self_T&&       self,
    util::StringLiteral extension_name
) -> Self_T&&
{
    self.m_required_capabilities.insert_extension(extension_name);
    return std::forward<Self_T>(self);
}

template <typename Self_T, feature_struct_c Features_T>
auto PhysicalDeviceFilter::require_features(this Self_T&& self, const Features_T& features)
    -> Self_T&&
{
    self.m_required_capabilities.insert_features(features);
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto PhysicalDeviceFilter::require_queue_flag(
    this Self_T&&           self,
    const vk::QueueFlagBits flag
) -> Self_T&&
{
    self.m_queue_flags |= flag;
    return std::forward<Self_T>(self);
}

template <typename Self_T>
auto PhysicalDeviceFilter::require_capabilities(
    this Self_T&&                     self,
    const PhysicalDeviceCapabilities& capabilities
) -> Self_T&&
{
    self.m_required_capabilities.insert(capabilities);
    return std::forward<Self_T>(self);
}

template <typename Self_T, typename... Args_T>
auto PhysicalDeviceFilter::add_custom_requirement(this Self_T&& self, Args_T&&... args)
    -> Self_T&&
    requires std::constructible_from<CustomRequirement, Args_T&&...>
{
    self.m_custom_requirements.emplace_back(std::forward<Args_T>(args)...);
    return std::forward<Self_T>(self);
}

}   // namespace kiln::gfx::vulkan
