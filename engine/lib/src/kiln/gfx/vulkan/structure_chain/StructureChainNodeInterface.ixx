module;

#include <concepts>

export module kiln.gfx.vulkan.structure_chain.StructureChainNodeInterface;

import vulkan;

namespace kiln::gfx::vulkan {

template <typename RootStruct_T>
class StructureChainNodeInterfaceBase {
public:
    using NextPtr = decltype(RootStruct_T::pNext);


    virtual ~StructureChainNodeInterfaceBase() = default;

    [[nodiscard]]
    virtual auto address() noexcept -> void* = 0;
    [[nodiscard]]
    virtual auto address() const noexcept -> const void* = 0;

    [[nodiscard]]
    virtual auto structure_type() const noexcept -> vk::StructureType = 0;

    [[nodiscard]]
    virtual auto next_pointer() noexcept -> NextPtr& = 0;
    [[nodiscard]]
    virtual auto next_pointer() const noexcept -> const NextPtr& = 0;
};

export template <typename RootStruct_T>
class StructureChainNodeInterface : public StructureChainNodeInterfaceBase<RootStruct_T> {
};

template <std::same_as<vk::PhysicalDeviceFeatures2> RootStruct_T>
class StructureChainNodeInterface<RootStruct_T>
    : public StructureChainNodeInterfaceBase<RootStruct_T>   //
{
public:
    [[nodiscard]]
    virtual auto operator!() const noexcept -> bool = 0;

    [[nodiscard]]
    virtual auto is_subset_of(const vk::BaseInStructure&) const -> bool = 0;

    virtual auto try_merge_into(vk::PhysicalDeviceVulkan11Features&) const
        -> vk::PhysicalDeviceVulkan11Features& = 0;
    virtual auto try_merge_into(vk::PhysicalDeviceVulkan12Features&) const
        -> vk::PhysicalDeviceVulkan12Features& = 0;
    virtual auto try_merge_into(vk::PhysicalDeviceVulkan13Features&) const
        -> vk::PhysicalDeviceVulkan13Features& = 0;
    virtual auto try_merge_into(vk::PhysicalDeviceVulkan14Features&) const
        -> vk::PhysicalDeviceVulkan14Features& = 0;

    virtual auto operator|=(const vk::BaseInStructure&)
        -> StructureChainNodeInterface& = 0;
    virtual auto operator&=(const vk::BaseInStructure&)
        -> StructureChainNodeInterface& = 0;
    virtual auto operator-=(const vk::BaseInStructure&)
        -> StructureChainNodeInterface& = 0;
};

}   // namespace kiln::gfx::vulkan
