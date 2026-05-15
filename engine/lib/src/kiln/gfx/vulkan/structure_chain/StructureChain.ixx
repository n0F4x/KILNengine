module;

#include <algorithm>
#include <flat_map>
#include <memory_resource>
#include <ranges>
#include <utility>
#include <vector>

#include "kiln/util/contract_macros.hpp"

export module kiln.gfx.vulkan.structure_chain.StructureChain;

import vulkan_hpp;

import kiln.gfx.vulkan.structure_chain.core_feature_struct_from_vulkan1x_c;
import kiln.gfx.vulkan.structure_chain.erase_physical_device_features;
import kiln.gfx.vulkan.structure_chain.extends_struct_c;
import kiln.gfx.vulkan.structure_chain.filter_physical_device_features;
import kiln.gfx.vulkan.structure_chain.is_empty_feature_struct;
import kiln.gfx.vulkan.structure_chain.match_physical_device_features;
import kiln.gfx.vulkan.structure_chain.merge_physical_device_features;
import kiln.gfx.vulkan.structure_chain.vulkan1x_feature_struct_c;
import kiln.util.concepts.naked;
import kiln.util.containers.Any;
import kiln.util.containers.OptionalRef;
import kiln.util.contracts;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;

namespace kiln::gfx::vulkan {

template <typename ErasedStruct_T>
class ErasedStructInterfaceMixin {
public:
    explicit ErasedStructInterfaceMixin(
        const util::AnyExtraVTableAccessor extra_vtable_accessor
    )
        : m_extra_vtable{ extra_vtable_accessor }
    {
    }

    [[nodiscard]]
    constexpr auto address(this ErasedStruct_T& self) -> void*
    {
        return self.m_extra_vtable(self).address_of(self);
    }

    [[nodiscard]]
    constexpr auto next_pointer(this ErasedStruct_T& self) -> void*&
    {
        return self.m_extra_vtable(self).next_pointer_of(self);
    }

    [[nodiscard]]
    constexpr auto const_next_pointer(this ErasedStruct_T& self) -> const void*&
    {
        return self.m_extra_vtable(self).const_next_pointer_of(self);
    }

    [[nodiscard]]
    constexpr auto empty(this const ErasedStruct_T& self) -> bool
    {
        PRECOND(self.m_extra_vtable(self).empty != nullptr);
        return self.m_extra_vtable(self).empty(self);
    }

    [[nodiscard]]
    constexpr auto matches(
        this const ErasedStruct_T& self,
        const vk::BaseInStructure& feature_struct
    ) -> bool
    {
        PRECOND(self.m_extra_vtable(self).matches != nullptr);
        return self.m_extra_vtable(self).matches(self, feature_struct);
    }

    constexpr auto merge(this ErasedStruct_T& self, const vk::BaseInStructure& features)
        -> void
    {
        PRECOND(self.m_extra_vtable(self).merge != nullptr);
        self.m_extra_vtable(self).merge(self, features);
    }

    constexpr auto try_merge_to_vulkan11_features(
        this const ErasedStruct_T&          self,
        vk::PhysicalDeviceVulkan11Features& vulkan11_features
    ) -> bool
    {
        if (self.m_extra_vtable(self).merge_to_Vulkan11_features != nullptr)
        {
            self.m_extra_vtable(self).merge_to_Vulkan11_features(self, vulkan11_features);
            return true;
        }
        return false;
    }

    constexpr auto try_merge_to_vulkan12_features(
        this const ErasedStruct_T&          self,
        vk::PhysicalDeviceVulkan12Features& vulkan12_features
    ) -> bool
    {
        if (self.m_extra_vtable(self).merge_to_Vulkan12_features != nullptr)
        {
            self.m_extra_vtable(self).merge_to_Vulkan12_features(self, vulkan12_features);
            return true;
        }
        return false;
    }

    constexpr auto try_merge_to_vulkan13_features(
        this const ErasedStruct_T&          self,
        vk::PhysicalDeviceVulkan13Features& vulkan13_features
    ) -> bool
    {
        if (self.m_extra_vtable(self).merge_to_Vulkan13_features != nullptr)
        {
            self.m_extra_vtable(self).merge_to_Vulkan13_features(self, vulkan13_features);
            return true;
        }
        return false;
    }

    constexpr auto try_merge_to_vulkan14_features(
        this const ErasedStruct_T&          self,
        vk::PhysicalDeviceVulkan14Features& vulkan14_features
    ) -> bool
    {
        if (self.m_extra_vtable(self).merge_to_Vulkan14_features != nullptr)
        {
            self.m_extra_vtable(self).merge_to_Vulkan14_features(self, vulkan14_features);
            return true;
        }
        return false;
    }

    constexpr auto filter(this ErasedStruct_T& self, const vk::BaseInStructure& features)
        -> void
    {
        PRECOND(self.m_extra_vtable(self).filter != nullptr);
        self.m_extra_vtable(self).filter(self, features);
    }

    constexpr auto erase(this ErasedStruct_T& self, const vk::BaseInStructure& features)
        -> void
    {
        PRECOND(self.m_extra_vtable(self).erase != nullptr);
        self.m_extra_vtable(self).erase(self, features);
    }

private:
    util::AnyExtraVTableAccessor m_extra_vtable;
};

template <typename ErasedStruct_T>
struct ErasedStructExtraVTable {
    template <typename Struct_T>
    struct Operations;

    using AddressFunc          = auto(ErasedStruct_T&) -> void*;
    using NextPointerFunc      = auto(ErasedStruct_T&) -> void*&;
    using ConstNextPointerFunc = auto(ErasedStruct_T&) -> const void*&;
    using EmptyFunc            = auto(const ErasedStruct_T&) -> bool;
    using MatchesFunc = auto(const ErasedStruct_T&, const vk::BaseInStructure&) -> bool;
    using MergeFunc   = auto(ErasedStruct_T&, const vk::BaseInStructure&) -> void;
    using MergeToVulkan11FeaturesFunc
        = auto(const ErasedStruct_T&, vk::PhysicalDeviceVulkan11Features&) -> void;
    using MergeToVulkan12FeaturesFunc
        = auto(const ErasedStruct_T&, vk::PhysicalDeviceVulkan12Features&) -> void;
    using MergeToVulkan13FeaturesFunc
        = auto(const ErasedStruct_T&, vk::PhysicalDeviceVulkan13Features&) -> void;
    using MergeToVulkan14FeaturesFunc
        = auto(const ErasedStruct_T&, vk::PhysicalDeviceVulkan14Features&) -> void;
    using FilterFunc = auto(ErasedStruct_T&, const vk::BaseInStructure&) -> void;
    using RemoveFunc = auto(ErasedStruct_T&, const vk::BaseInStructure&) -> void;

    std::reference_wrapper<AddressFunc>             address_of;
    std::add_pointer_t<NextPointerFunc>             next_pointer_of;
    std::add_pointer_t<ConstNextPointerFunc>        const_next_pointer_of;
    std::add_pointer_t<EmptyFunc>                   empty{};
    std::add_pointer_t<MatchesFunc>                 matches{};
    std::add_pointer_t<MergeFunc>                   merge{};
    std::add_pointer_t<MergeToVulkan11FeaturesFunc> merge_to_Vulkan11_features{};
    std::add_pointer_t<MergeToVulkan12FeaturesFunc> merge_to_Vulkan12_features{};
    std::add_pointer_t<MergeToVulkan13FeaturesFunc> merge_to_Vulkan13_features{};
    std::add_pointer_t<MergeToVulkan14FeaturesFunc> merge_to_Vulkan14_features{};
    std::add_pointer_t<FilterFunc>                  filter{};
    std::add_pointer_t<RemoveFunc>                  erase{};
};

template <typename ErasedStruct_T>
template <typename Struct_T>
struct ErasedStructExtraVTable<ErasedStruct_T>::Operations {
    [[nodiscard]]
    constexpr static auto address_of(ErasedStruct_T& that) -> void*
    {
        return &util::any_cast<Struct_T>(that);
    }

    [[nodiscard]]
    constexpr static auto next_pointer_of(ErasedStruct_T& that) -> void*&
        requires(!std::is_const_v<std::remove_pointer_t<decltype(Struct_T::pNext)>>)
    {
        return util::any_cast<Struct_T>(that).pNext;
    }

    [[nodiscard]]
    constexpr static auto const_next_pointer_of(ErasedStruct_T& that) -> const void*&
        requires(std::is_const_v<std::remove_pointer_t<decltype(Struct_T::pNext)>>)
    {
        return util::any_cast<Struct_T>(that).pNext;
    }

    [[nodiscard]]
    constexpr static auto empty(const ErasedStruct_T& that) -> bool
    {
        return is_empty_feature_struct(util::any_cast<Struct_T>(that));
    }

    [[nodiscard]]
    constexpr static auto matches(
        const ErasedStruct_T&      that,
        const vk::BaseInStructure& other
    ) -> bool
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        PRECOND(Struct_T::structureType == other.sType);

        return match_physical_device_features(
            util::any_cast<Struct_T>(that),
            reinterpret_cast<const Struct_T&>(other)
        );
    }

    constexpr static auto merge(ErasedStruct_T& that, const vk::BaseInStructure& other)
        -> void
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        PRECOND(Struct_T::structureType == other.sType);

        merge_physical_device_features(
            util::any_cast<Struct_T>(that),
            reinterpret_cast<const Struct_T&>(other)
        );
    }

    constexpr static auto merge_to_vulkan11_features(
        const ErasedStruct_T&               that,
        vk::PhysicalDeviceVulkan11Features& vulkan11_features
    ) -> void
        requires core_feature_struct_from_vulkan11_c<Struct_T>
    {
        merge_physical_device_features(vulkan11_features, util::any_cast<Struct_T>(that));
    }

    constexpr static auto merge_to_vulkan12_features(
        const ErasedStruct_T&               that,
        vk::PhysicalDeviceVulkan12Features& vulkan12_features
    ) -> void
        requires core_feature_struct_from_vulkan12_c<Struct_T>
    {
        merge_physical_device_features(vulkan12_features, util::any_cast<Struct_T>(that));
    }

    constexpr static auto merge_to_vulkan13_features(
        const ErasedStruct_T&               that,
        vk::PhysicalDeviceVulkan13Features& vulkan13_features
    ) -> void
        requires core_feature_struct_from_vulkan13_c<Struct_T>
    {
        merge_physical_device_features(vulkan13_features, util::any_cast<Struct_T>(that));
    }

    constexpr static auto merge_to_vulkan14_features(
        const ErasedStruct_T&               that,
        vk::PhysicalDeviceVulkan14Features& vulkan14_features
    ) -> void
        requires core_feature_struct_from_vulkan14_c<Struct_T>
    {
        merge_physical_device_features(vulkan14_features, util::any_cast<Struct_T>(that));
    }

    constexpr static auto filter(ErasedStruct_T& that, const vk::BaseInStructure& other)
        -> void
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        PRECOND(Struct_T::structureType == other.sType);

        filter_physical_device_features(
            util::any_cast<Struct_T>(that),
            reinterpret_cast<const Struct_T&>(other)
        );
    }

    constexpr static auto erase(ErasedStruct_T& that, const vk::BaseInStructure& other)
        -> void
        requires extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>
    {
        PRECOND(Struct_T::structureType == other.sType);

        erase_physical_device_features(
            util::any_cast<Struct_T>(that),
            reinterpret_cast<const Struct_T&>(other)
        );
    }

    constexpr static ErasedStructExtraVTable vtable{
        .address_of      = address_of,
        .next_pointer_of = [] -> auto
        {
            if constexpr (std::is_const_v<std::remove_pointer_t<decltype(Struct_T::pNext)>>)
            {
                return nullptr;
            }
            else
            {
                return next_pointer_of;
            }
        }(),
        .const_next_pointer_of = [] -> auto
        {
            if constexpr (std::is_const_v<std::remove_pointer_t<decltype(Struct_T::pNext)>>)
            {
                return const_next_pointer_of;
            }
            else
            {
                return nullptr;
            }
        }(),
        .empty = [] -> auto
        {
            if constexpr (extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>)
            {
                return empty;
            }
            else
            {
                return nullptr;
            }
        }(),
        .matches = [] -> auto
        {
            if constexpr (extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>)
            {
                return matches;
            }
            else
            {
                return nullptr;
            }
        }(),
        .merge = [] -> auto
        {
            if constexpr (extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>)
            {
                return merge;
            }
            else
            {
                return nullptr;
            }
        }(),
        .merge_to_Vulkan11_features = [] -> auto
        {
            if constexpr (core_feature_struct_from_vulkan11_c<Struct_T>)
            {
                return merge_to_vulkan11_features;
            }
            else
            {
                return nullptr;
            }
        }(),
        .merge_to_Vulkan12_features = [] -> auto
        {
            if constexpr (core_feature_struct_from_vulkan12_c<Struct_T>)
            {
                return merge_to_vulkan12_features;
            }
            else
            {
                return nullptr;
            }
        }(),
        .merge_to_Vulkan13_features = [] -> auto
        {
            if constexpr (core_feature_struct_from_vulkan13_c<Struct_T>)
            {
                return merge_to_vulkan13_features;
            }
            else
            {
                return nullptr;
            }
        }(),
        .merge_to_Vulkan14_features = [] -> auto
        {
            if constexpr (core_feature_struct_from_vulkan14_c<Struct_T>)
            {
                return merge_to_vulkan14_features;
            }
            else
            {
                return nullptr;
            }
        }(),
        .filter = [] -> auto
        {
            if constexpr (extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>)
            {
                return filter;
            }
            else
            {
                return nullptr;
            }
        }(),
        .erase = [] -> auto
        {
            if constexpr (extends_struct_c<Struct_T, vk::PhysicalDeviceFeatures2>)
            {
                return erase;
            }
            else
            {
                return nullptr;
            }
        }(),
    };
};

using ErasedStruct = util::BasicAny<util::DefaultAnyTraits<
    false,
    util::default_any_size(),
    util::default_any_alignment(),
    util::DefaultAnyPolicy,
    ErasedStructInterfaceMixin,
    ErasedStructExtraVTable>>;

export template <util::naked_c RootStruct_T>
class StructureChain {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;
    using NextPtr        = decltype(RootStruct_T::pNext);


    StructureChain(const StructureChain&);
    StructureChain(const StructureChain&, const allocator_type& allocator);
    StructureChain(StructureChain&&) noexcept;
    StructureChain(StructureChain&&, const allocator_type& allocator);

    explicit StructureChain() = default;
    explicit StructureChain(const allocator_type& allocator);


    auto operator=(const StructureChain&) -> StructureChain&;
    auto operator=(StructureChain&&) noexcept -> StructureChain&;


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    template <typename Self_T>
    [[nodiscard]]
    constexpr auto root(this Self_T&&) -> util::forward_like_t<RootStruct_T, Self_T>;

    template <extends_struct_c<RootStruct_T> Struct_T>
    [[nodiscard]]
    constexpr auto contains() const -> bool;
    template <extends_struct_c<RootStruct_T> Struct_T, typename Self_T>
    [[nodiscard]]
    constexpr auto find(
        this Self_T&
    ) -> util::OptionalRef<util::const_like_t<Struct_T, std::remove_reference_t<Self_T>>>;
    template <extends_struct_c<RootStruct_T> Struct_T, typename Self_T>
    [[nodiscard]]
    constexpr auto at(this Self_T&&) -> util::forward_like_t<Struct_T, Self_T>;

    [[nodiscard]]
    constexpr auto matches(
        const vk::PhysicalDeviceFeatures2& physical_device_features
    ) const -> bool
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    template <extends_struct_c<vk::PhysicalDeviceFeatures2> Features_T>
    constexpr auto merge(const Features_T& features) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;
    constexpr auto merge(const StructureChain<vk::PhysicalDeviceFeatures2>& other) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    template <vulkan1x_feature_struct_c Vulkan1XFeatures_T>
    constexpr auto erase_and_merge_features_to_vulkan1x_feature_struct(
        Vulkan1XFeatures_T& vulkan1X_features
    ) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    constexpr auto erase_unsupported_features(
        const vk::PhysicalDeviceFeatures2& supported_features
    ) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

    constexpr auto erase_features(const vk::PhysicalDeviceFeatures& features) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;
    template <extends_struct_c<vk::PhysicalDeviceFeatures2> Features_T>
    constexpr auto erase_features(const Features_T& features) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;
    constexpr auto erase_features(
        const vk::PhysicalDeviceFeatures2& physical_device_features
    ) -> void
        requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>;

private:
    RootStruct_T m_root_struct{};
    std::flat_map<
        vk::StructureType,
        ErasedStruct,
        std::less<>,
        std::pmr::vector<vk::StructureType>,
        std::pmr::vector<ErasedStruct>>
        m_chain;

    template <typename Self_T, extends_struct_c<RootStruct_T> Struct_T>
    constexpr auto operator[](this Self_T&&, std::in_place_type_t<Struct_T>)
        -> util::forward_like_t<Struct_T, Self_T>;

    constexpr auto connect() -> void;
};

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan {

template <util::naked_c RootStruct_T>
StructureChain<RootStruct_T>::StructureChain(const StructureChain& other)
    : m_root_struct{ other.m_root_struct },
      m_chain{ other.m_chain }
{
    connect();
}

template <util::naked_c RootStruct_T>
StructureChain<RootStruct_T>::StructureChain(
    const StructureChain& other,
    const allocator_type& allocator
)
    : m_root_struct{ other.m_root_struct },
      m_chain{ other.m_chain, allocator }
{
    connect();
}

template <util::naked_c RootStruct_T>
StructureChain<RootStruct_T>::StructureChain(StructureChain&& other) noexcept
    : m_root_struct{ std::move(other.m_root_struct) },
      m_chain{ std::move(other.m_chain) }
{
    connect();
}

template <util::naked_c RootStruct_T>
StructureChain<RootStruct_T>::StructureChain(
    StructureChain&&      other,
    const allocator_type& allocator
)
    : m_root_struct{ std::move(other.m_root_struct) },
      m_chain{ std::move(other.m_chain), allocator }
{
    connect();
}

template <util::naked_c RootStruct_T>
StructureChain<RootStruct_T>::StructureChain(const allocator_type& allocator)
    : m_chain{ allocator }
{
}

template <util::naked_c RootStruct_T>
auto StructureChain<RootStruct_T>::operator=(const StructureChain& other)
    -> StructureChain&
{
    if (this == &other)
    {
        return *this;
    }

    m_root_struct = other.m_root_struct;
    m_chain       = other.m_chain;

    connect();

    return *this;
}

template <util::naked_c RootStruct_T>
auto StructureChain<RootStruct_T>::operator=(StructureChain&& other) noexcept
    -> StructureChain&
{
    if (this == &other)
    {
        return *this;
    }

    m_root_struct = std::move(other.m_root_struct);
    m_chain       = std::move(other.m_chain);

    connect();

    return *this;
}

template <util::naked_c RootStruct_T>
auto StructureChain<RootStruct_T>::get_allocator() const noexcept -> allocator_type
{
    return m_chain.keys().get_allocator();
}

template <util::naked_c RootStruct_T>
template <typename Self_T>
constexpr auto StructureChain<RootStruct_T>::root(this Self_T&& self)
    -> util::forward_like_t<RootStruct_T, Self_T>
{
    return std::forward_like<Self_T>(self.m_root_struct);
}

template <util::naked_c RootStruct_T>
template <extends_struct_c<RootStruct_T> Struct_T>
constexpr auto StructureChain<RootStruct_T>::contains() const -> bool
{
    return m_chain.contains(Struct_T::structureType);
}

template <util::naked_c RootStruct_T>
template <extends_struct_c<RootStruct_T> Struct_T, typename Self_T>
constexpr auto StructureChain<RootStruct_T>::find(this Self_T& self)
    -> util::OptionalRef<util::const_like_t<Struct_T, std::remove_reference_t<Self_T>>>
{
    const auto iter{ self.m_chain.find(Struct_T::structureType) };
    if (iter == self.m_chain.cend())
    {
        return std::nullopt;
    }

    return util::any_cast<Struct_T>(iter->second);
}

template <util::naked_c RootStruct_T>
template <extends_struct_c<RootStruct_T> Struct_T, typename Self_T>
constexpr auto StructureChain<RootStruct_T>::at(this Self_T&& self)
    -> util::forward_like_t<Struct_T, Self_T>
{
    PRECOND(self.template contains<Struct_T>());

    return util::any_cast<Struct_T>(
        std::forward_like<Self_T>(self.m_map.at(Struct_T::structureType))
    );
}

template <util::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::matches(
    const vk::PhysicalDeviceFeatures2& physical_device_features
) const -> bool
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    if (!match_physical_device_features(
            m_root_struct.features,
            physical_device_features.features
        ))
    {
        return false;
    }

    for (const auto* supported_feature_struct{
             static_cast<const vk::BaseInStructure*>(physical_device_features.pNext) };
         supported_feature_struct != nullptr;
         supported_feature_struct = supported_feature_struct->pNext)
    {
        if (const auto iter = m_chain.find(supported_feature_struct->sType);
            iter != m_chain.cend() && !iter->second.matches(*supported_feature_struct))
        {
            return false;
        }
    }

    return true;
}

template <util::naked_c RootStruct_T>
template <extends_struct_c<vk::PhysicalDeviceFeatures2> Features_T>
constexpr auto StructureChain<RootStruct_T>::merge(const Features_T& features) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    merge_physical_device_features(operator[](std::in_place_type<Features_T>), features);
}

template <util::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::merge(
    const StructureChain<vk::PhysicalDeviceFeatures2>& other
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    merge_physical_device_features(m_root_struct.features, other.m_root_struct.features);

    bool needs_reconnect{};
    for (const auto& [incoming_sType, incoming_features] : other.m_chain)
    {
        const auto [iter, inserted]
            = m_chain.try_emplace(incoming_sType, incoming_features);

        iter->second.merge(
            util::reinterpret_any_cast<vk::BaseInStructure>(incoming_features)
        );

        needs_reconnect |= inserted;
    }

    if (needs_reconnect)
    {
        connect();
    }
}

template <util::naked_c RootStruct_T>
template <vulkan1x_feature_struct_c Vulkan1XFeatures_T>
constexpr auto
    StructureChain<RootStruct_T>::erase_and_merge_features_to_vulkan1x_feature_struct(
        Vulkan1XFeatures_T& vulkan1X_features
    ) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    std::vector<vk::StructureType> marked_for_removal;

    for (const auto [sType, erased_features] : m_chain)
    {
        if constexpr (std::is_same_v<Vulkan1XFeatures_T, vk::PhysicalDeviceVulkan11Features>)
        {
            if (erased_features.try_merge_to_vulkan11_features(vulkan1X_features))
            {
                marked_for_removal.push_back(sType);
            }
        }
        else if constexpr (std::is_same_v<
                               Vulkan1XFeatures_T,
                               vk::PhysicalDeviceVulkan12Features>)
        {
            if (erased_features.try_merge_to_vulkan12_features(vulkan1X_features))
            {
                marked_for_removal.push_back(sType);
            }
        }
        else if constexpr (std::is_same_v<
                               Vulkan1XFeatures_T,
                               vk::PhysicalDeviceVulkan13Features>)
        {
            if (erased_features.try_merge_to_vulkan13_features(vulkan1X_features))
            {
                marked_for_removal.push_back(sType);
            }
        }
        else if constexpr (std::is_same_v<
                               Vulkan1XFeatures_T,
                               vk::PhysicalDeviceVulkan14Features>)
        {
            if (erased_features.try_merge_to_vulkan14_features(vulkan1X_features))
            {
                marked_for_removal.push_back(sType);
            }
        }
        else
        {
            static_assert(false, "Vulkan 1.5 or higher is not supported");
        }
    }

    // NOLINTNEXTLINE(*-identifier-naming)
    for (const vk::StructureType sType : marked_for_removal)
    {
        m_chain.erase(sType);
    }

    if (!marked_for_removal.empty())
    {
        connect();
    }
}

template <util::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::erase_unsupported_features(
    const vk::PhysicalDeviceFeatures2& supported_features
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    filter_physical_device_features(m_root_struct.features, supported_features.features);

    bool needs_reconnect{};

    for (const auto* incoming_feature_struct{
             static_cast<const vk::BaseInStructure*>(supported_features.pNext) };
         incoming_feature_struct != nullptr;
         incoming_feature_struct = incoming_feature_struct->pNext)
    {
        if (const auto iter = m_chain.find(incoming_feature_struct->sType);
            iter != m_chain.end())
        {
            iter->second.filter(*incoming_feature_struct);
            if (iter->second.empty())
            {
                m_chain.erase(iter);
                needs_reconnect |= true;
            }
        }
    }

    if (needs_reconnect)
    {
        connect();
    }
}

template <util::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::erase_features(
    const vk::PhysicalDeviceFeatures& features
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    erase_physical_device_features(m_root_struct.features, features);
}

template <util::naked_c RootStruct_T>
template <extends_struct_c<vk::PhysicalDeviceFeatures2> Features_T>
constexpr auto StructureChain<RootStruct_T>::erase_features(const Features_T& features)
    -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    if (const auto iter = m_chain.find(Features_T::structureType); iter != m_chain.cend())
    {
        iter->second.erase(reinterpret_cast<const vk::BaseInStructure&>(features));
        if (iter->second.empty())
        {
            m_chain.erase(iter);
            connect();
        }
    }
}

template <util::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::erase_features(
    const vk::PhysicalDeviceFeatures2& physical_device_features
) -> void
    requires std::same_as<RootStruct_T, vk::PhysicalDeviceFeatures2>
{
    erase_physical_device_features(
        m_root_struct.features,
        physical_device_features.features
    );

    bool needs_reconnect{};

    for (const auto* incoming_feature_struct{
             static_cast<const vk::BaseInStructure*>(physical_device_features.pNext) };
         incoming_feature_struct != nullptr;
         incoming_feature_struct = incoming_feature_struct->pNext)
    {
        if (const auto iter = m_chain.find(incoming_feature_struct->sType);
            iter != m_chain.end())
        {
            iter->second.erase(*incoming_feature_struct);
            if (iter->second.empty())
            {
                m_chain.erase(iter);
                needs_reconnect |= true;
            }
        }
    }

    if (needs_reconnect)
    {
        connect();
    }
}

template <util::naked_c RootStruct_T>
template <typename Self_T, extends_struct_c<RootStruct_T> Struct_T>
constexpr auto StructureChain<RootStruct_T>::operator[](
    this Self_T&& self,
    std::in_place_type_t<Struct_T>
) -> util::forward_like_t<Struct_T, Self_T>
{
    const auto [iter, inserted]
        = self.m_chain.try_emplace(Struct_T::structureType, std::in_place_type<Struct_T>);

    if (inserted)
    {
        self.connect();
    }

    return util::any_cast<Struct_T>(std::forward_like<Self_T>(iter->second));
}

template <util::naked_c RootStruct_T>
constexpr auto StructureChain<RootStruct_T>::connect() -> void
{
    std::reference_wrapper<NextPtr> previous_pointer{ m_root_struct.pNext };
    for (ErasedStruct& next_struct : m_chain | std::views::values)
    {
        previous_pointer.get() = next_struct.address();
        if constexpr (std::is_const_v<std::remove_pointer_t<NextPtr>>)
        {
            previous_pointer = next_struct.const_next_pointer();
        }
        else
        {
            previous_pointer = next_struct.next_pointer();
        }
    }
    previous_pointer.get() = nullptr;
}

}   // namespace kiln::gfx::vulkan
