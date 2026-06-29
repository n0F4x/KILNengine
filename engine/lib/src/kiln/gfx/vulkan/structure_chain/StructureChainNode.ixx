module;

#include <concepts>

#include "kiln/util/contract_macros.hpp"

export module kiln.gfx.vulkan.structure_chain.StructureChainNode;

import vulkan;

import kiln.gfx.vulkan.structure_chain.core_feature_struct_from_vulkan1x_c;
import kiln.gfx.vulkan.structure_chain.erase_physical_device_features;
import kiln.gfx.vulkan.structure_chain.extends_struct_c;
import kiln.gfx.vulkan.structure_chain.filter_physical_device_features;
import kiln.gfx.vulkan.structure_chain.is_empty_feature_struct;
import kiln.gfx.vulkan.structure_chain.match_physical_device_features;
import kiln.gfx.vulkan.structure_chain.merge_physical_device_features;
import kiln.gfx.vulkan.structure_chain.StructureChainNodeInterface;
import kiln.util.contracts;

namespace kiln::gfx::vulkan {

template <typename RootStruct_T, extends_struct_c<RootStruct_T> Struct_T>
class StructureChainNodeBase : public StructureChainNodeInterface<RootStruct_T> {
public:
    using NextPtr = StructureChainNodeInterface<RootStruct_T>::NextPtr;

    [[nodiscard]]
    constexpr auto address() noexcept -> void* final
    {
        return &m_structure;
    }

    [[nodiscard]]
    constexpr auto address() const noexcept -> const void* final
    {
        return &m_structure;
    }

    [[nodiscard]]
    constexpr auto structure_type() const noexcept -> vk::StructureType final
    {
        return Struct_T::structureType;
    }

    [[nodiscard]]
    constexpr auto next_pointer() noexcept -> NextPtr& final
    {
        return m_structure.pNext;
    }

    [[nodiscard]]
    constexpr auto next_pointer() const noexcept -> const NextPtr& final
    {
        return m_structure.pNext;
    }

    [[nodiscard]]
    constexpr auto structure() noexcept -> Struct_T&
    {
        return m_structure;
    }

    [[nodiscard]]
    constexpr auto structure() const noexcept -> const Struct_T&
    {
        return m_structure;
    }

private:
    Struct_T m_structure{};
};

export template <typename RootStruct_T, extends_struct_c<RootStruct_T> Struct_T>
class StructureChainNode : public StructureChainNodeBase<RootStruct_T, Struct_T> {};

template <
    std::same_as<vk::PhysicalDeviceFeatures2> RootStruct_T,
    extends_struct_c<RootStruct_T>            Struct_T>
class StructureChainNode<RootStruct_T, Struct_T>
    : public StructureChainNodeBase<RootStruct_T, Struct_T> {
public:
    [[nodiscard]]
    constexpr auto operator!() const noexcept -> bool final
    {
        return is_empty_feature_struct(this->structure());
    }

    [[nodiscard]]
    constexpr auto is_subset_of(const vk::BaseInStructure& other) const -> bool final
    {
        PRECOND(this->structure_type() == other.sType);

        return match_physical_device_features(
            this->structure(),
            reinterpret_cast<const Struct_T&>(other)
        );
    }

    constexpr auto try_merge_into(
        vk::PhysicalDeviceVulkan11Features& vulkan11_features
    ) const -> vk::PhysicalDeviceVulkan11Features& final
    {
        if constexpr (core_feature_struct_from_vulkan11_c<Struct_T>)
        {
            return merge_physical_device_features(vulkan11_features, this->structure());
        }
        else
        {
            return vulkan11_features;
        }
    }

    constexpr auto try_merge_into(
        vk::PhysicalDeviceVulkan12Features& vulkan12_features
    ) const -> vk::PhysicalDeviceVulkan12Features& final
    {
        if constexpr (core_feature_struct_from_vulkan12_c<Struct_T>)
        {
            return merge_physical_device_features(vulkan12_features, this->structure());
        }
        else
        {
            return vulkan12_features;
        }
    }

    constexpr auto try_merge_into(
        vk::PhysicalDeviceVulkan13Features& vulkan13_features
    ) const -> vk::PhysicalDeviceVulkan13Features& final
    {
        if constexpr (core_feature_struct_from_vulkan13_c<Struct_T>)
        {
            return merge_physical_device_features(vulkan13_features, this->structure());
        }
        else
        {
            return vulkan13_features;
        }
    }

    constexpr auto try_merge_into(
        vk::PhysicalDeviceVulkan14Features& vulkan14_features
    ) const -> vk::PhysicalDeviceVulkan14Features& final
    {
        if constexpr (core_feature_struct_from_vulkan14_c<Struct_T>)
        {
            return merge_physical_device_features(vulkan14_features, this->structure());
        }
        else
        {
            return vulkan14_features;
        }
    }

    constexpr auto operator|=(const vk::BaseInStructure& other)
        -> StructureChainNode& final
    {
        PRECOND(this->structure_type() == other.sType);

        merge_physical_device_features(
            this->structure(),
            reinterpret_cast<const Struct_T&>(other)
        );

        return *this;
    }

    constexpr auto operator&=(const vk::BaseInStructure& other)
        -> StructureChainNode& final
    {
        PRECOND(this->structure_type() == other.sType);

        filter_physical_device_features(
            this->structure(),
            reinterpret_cast<const Struct_T&>(other)
        );

        return *this;
    }

    constexpr auto operator-=(const vk::BaseInStructure& other)
        -> StructureChainNode& final
    {
        PRECOND(this->structure_type() == other.sType);

        erase_physical_device_features(
            this->structure(),
            reinterpret_cast<const Struct_T&>(other)
        );

        return *this;
    }
};

}   // namespace kiln::gfx::vulkan
