module;

#include <algorithm>
#include <cstdint>
#include <span>
#include <utility>
#include <vector>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.vulkan.PhysicalDeviceCapabilities;

import vulkan_hpp;

import kiln.util.contracts;

import kiln.gfx.vulkan.extension_inspection.try_promote_extension_to_vulkan1x;
import kiln.gfx.vulkan.structure_chain.merge_physical_device_features;
import kiln.gfx.vulkan.structure_chain.erase_physical_device_features;
import kiln.gfx.vulkan.structure_chain.StructureChain;
import kiln.util.StringLiteral;

namespace kiln::gfx::vulkan {

PhysicalDeviceCapabilities::PhysicalDeviceCapabilities(
    const PhysicalDeviceCapabilities& other,
    const allocator_type&             allocator
)
    : m_version{ other.m_version },
      m_extension_names{ other.m_extension_names, allocator },
      m_features{ other.m_features, allocator }
{
}

PhysicalDeviceCapabilities::PhysicalDeviceCapabilities(
    PhysicalDeviceCapabilities&& other,
    const allocator_type&        allocator
)
    : m_version{ other.m_version },
      m_extension_names{ std::move(other.m_extension_names), allocator },
      m_features{ std::move(other.m_features), allocator }
{
}

PhysicalDeviceCapabilities::PhysicalDeviceCapabilities(const allocator_type& allocator)
    : m_extension_names{ allocator },
      m_features{ allocator }
{
}

auto PhysicalDeviceCapabilities::get_allocator() const noexcept -> allocator_type
{
    return m_extension_names.get_allocator();
}

auto PhysicalDeviceCapabilities::version() const noexcept -> uint32_t
{
    return m_version;
}

auto PhysicalDeviceCapabilities::extensions() const noexcept
    -> std::span<const util::StringLiteral>
{
    return m_extension_names;
}

auto PhysicalDeviceCapabilities::features_chain() const noexcept
    -> const StructureChain<vk::PhysicalDeviceFeatures2>&
{
    return m_features;
}

auto PhysicalDeviceCapabilities::supported_by(
    const vk::raii::PhysicalDevice& physical_device
) const -> bool
{
    if (physical_device.getProperties2().properties.apiVersion < m_version)
    {
        return false;
    }

    const std::vector<vk::ExtensionProperties> supported_extension_properties{
        physical_device.enumerateDeviceExtensionProperties()
    };
    if (std::ranges::any_of(
            m_extension_names,
            [&supported_extension_properties]                    //
            (const util::StringLiteral extension_name) -> bool   //
            {
                return std::ranges::none_of(
                    supported_extension_properties,
                    [extension_name](
                        const vk::ExtensionProperties& extension_properties
                    ) -> bool
                    {
                        return extension_name
                            == util::StringLiteral::unsafe_create(
                                   extension_properties.extensionName
                            );
                    }
                );
            }
        ))
    {
        return false;
    }

    StructureChain<vk::PhysicalDeviceFeatures2> supported_features{ m_features };
    physical_device.getDispatcher()
        ->vkGetPhysicalDeviceFeatures2(*physical_device, supported_features.root());
    if (!m_features.matches(supported_features.root()))
    {
        return false;
    }

    return true;
}

auto PhysicalDeviceCapabilities::filter_uncontained_features(
    vk::PhysicalDeviceFeatures& features
) const -> void
{
    erase_physical_device_features(features, m_features.root().features);
}

auto PhysicalDeviceCapabilities::upgrade_version(const uint32_t new_version) -> void
{
    PRECOND(
        new_version < vk::makeApiVersion(0, 1, 5, 0),
        "Only Vulkan versions lower than 1.5 are supported"
    );

    if (new_version > m_version)
    {
        if (const uint32_t new_minor_version{ vk::apiVersionMinor(new_version) };
            new_minor_version > vk::apiVersionMinor(m_version))
        {
            if (new_minor_version == 1)
            {
                upgrade_to_Vulkan11();
            }
            else if (new_minor_version == 2)
            {
                upgrade_to_Vulkan12();
            }
            else if (new_minor_version == 3)
            {
                upgrade_to_Vulkan13();
            }
            else if (new_minor_version == 4)
            {
                upgrade_to_Vulkan14();
            }
        }

        m_version = new_version;
    }
}

auto PhysicalDeviceCapabilities::insert_extension(const util::StringLiteral extension_name)
    -> void
{
    if (m_version >= vk::ApiVersion11)
    {
        if (vk::PhysicalDeviceVulkan11Features vulkan11_features{};
            try_promote_extension_to_vulkan11(extension_name, vulkan11_features))
        {
            if (vulkan11_features != vk::PhysicalDeviceVulkan11Features{})
            {
                insert_features(vulkan11_features);
            }
            return;
        }
    }
    if (m_version >= vk::ApiVersion12)
    {
        if (vk::PhysicalDeviceVulkan12Features vulkan12_features{};
            try_promote_extension_to_vulkan12(extension_name, vulkan12_features))
        {
            if (vulkan12_features != vk::PhysicalDeviceVulkan12Features{})
            {
                insert_features(vulkan12_features);
            }
            return;
        }
    }
    if (m_version >= vk::ApiVersion13)
    {
        if (vk::PhysicalDeviceVulkan13Features vulkan13_features{};
            try_promote_extension_to_vulkan13(extension_name, vulkan13_features))
        {
            if (vulkan13_features != vk::PhysicalDeviceVulkan13Features{})
            {
                insert_features(vulkan13_features);
            }
            return;
        }
    }
    if (m_version >= vk::ApiVersion14)
    {
        if (vk::PhysicalDeviceVulkan14Features vulkan14_features{};
            try_promote_extension_to_vulkan14(extension_name, vulkan14_features))
        {
            if (vulkan14_features != vk::PhysicalDeviceVulkan14Features{})
            {
                insert_features(vulkan14_features);
            }
            return;
        }
    }

    if (!std::ranges::contains(m_extension_names, extension_name))
    {
        m_extension_names.push_back(extension_name);
    }
}

auto PhysicalDeviceCapabilities::insert_features(
    const vk::PhysicalDeviceFeatures& features
) -> void
{
    merge_physical_device_features(m_features.root().features, features);
}

auto PhysicalDeviceCapabilities::insert_features(
    const vk::PhysicalDeviceVulkan11Features& features
) -> void
{
    upgrade_version(vk::ApiVersion11);

    m_features.merge(features);
}

auto PhysicalDeviceCapabilities::insert_features(
    const vk::PhysicalDeviceVulkan12Features& features
) -> void
{
    upgrade_version(vk::ApiVersion12);

    m_features.merge(features);
}

auto PhysicalDeviceCapabilities::insert_features(
    const vk::PhysicalDeviceVulkan13Features& features
) -> void
{
    upgrade_version(vk::ApiVersion13);

    m_features.merge(features);
}

auto PhysicalDeviceCapabilities::insert_features(
    const vk::PhysicalDeviceVulkan14Features& features
) -> void
{
    upgrade_version(vk::ApiVersion14);

    m_features.merge(features);
}

auto PhysicalDeviceCapabilities::insert_features(
    StructureChain<vk::PhysicalDeviceFeatures2> features
) -> void
{
    if (m_version < vk::ApiVersion11
        && features.contains<vk::PhysicalDeviceVulkan11Features>())
    {
        upgrade_to_Vulkan11();
    }
    if (m_version < vk::ApiVersion12
        && features.contains<vk::PhysicalDeviceVulkan12Features>())
    {
        upgrade_to_Vulkan12();
    }
    if (m_version < vk::ApiVersion13
        && features.contains<vk::PhysicalDeviceVulkan13Features>())
    {
        upgrade_to_Vulkan13();
    }
    if (m_version < vk::ApiVersion14
        && features.contains<vk::PhysicalDeviceVulkan14Features>())
    {
        upgrade_to_Vulkan14();
    }

    if (m_version >= vk::ApiVersion11)
    {
        vk::PhysicalDeviceVulkan11Features vulkan11_features{};
        features.erase_and_merge_features_to_vulkan1x_feature_struct(vulkan11_features);
        features.merge(vulkan11_features);
    }
    if (m_version >= vk::ApiVersion12)
    {
        vk::PhysicalDeviceVulkan12Features vulkan12_features{};
        features.erase_and_merge_features_to_vulkan1x_feature_struct(vulkan12_features);
        features.merge(vulkan12_features);
    }
    if (m_version >= vk::ApiVersion13)
    {
        vk::PhysicalDeviceVulkan13Features vulkan13_features{};
        features.erase_and_merge_features_to_vulkan1x_feature_struct(vulkan13_features);
        features.merge(vulkan13_features);
    }
    if (m_version >= vk::ApiVersion14)
    {
        vk::PhysicalDeviceVulkan14Features vulkan14_features{};
        features.erase_and_merge_features_to_vulkan1x_feature_struct(vulkan14_features);
        features.merge(vulkan14_features);
    }

    m_features.merge(features);
}

auto PhysicalDeviceCapabilities::insert(const PhysicalDeviceCapabilities& other) -> void
{
    upgrade_version(other.version());
    for (const util::StringLiteral extension : other.extensions())
    {
        insert_extension(extension);
    }
    insert_features(other.features_chain());
}

auto PhysicalDeviceCapabilities::erase_extension(const util::StringLiteral extension_name)
    -> void
{
    if (m_version >= vk::ApiVersion11)
    {
        if (vk::PhysicalDeviceVulkan11Features vulkan11_features{};
            try_promote_extension_to_vulkan11(extension_name, vulkan11_features))
        {
            if (vulkan11_features != vk::PhysicalDeviceVulkan11Features{})
            {
                erase_features(vulkan11_features);
            }
            return;
        }
    }
    if (m_version >= vk::ApiVersion12)
    {
        if (vk::PhysicalDeviceVulkan12Features vulkan12_features{};
            try_promote_extension_to_vulkan12(extension_name, vulkan12_features))
        {
            if (vulkan12_features != vk::PhysicalDeviceVulkan12Features{})
            {
                erase_features(vulkan12_features);
            }
            return;
        }
    }
    if (m_version >= vk::ApiVersion13)
    {
        if (vk::PhysicalDeviceVulkan13Features vulkan13_features{};
            try_promote_extension_to_vulkan13(extension_name, vulkan13_features))
        {
            if (vulkan13_features != vk::PhysicalDeviceVulkan13Features{})
            {
                erase_features(vulkan13_features);
            }
            return;
        }
    }
    if (m_version >= vk::ApiVersion14)
    {
        if (vk::PhysicalDeviceVulkan14Features vulkan14_features{};
            try_promote_extension_to_vulkan14(extension_name, vulkan14_features))
        {
            if (vulkan14_features != vk::PhysicalDeviceVulkan14Features{})
            {
                erase_features(vulkan14_features);
            }
            return;
        }
    }

    std::erase(m_extension_names, extension_name);
}

auto PhysicalDeviceCapabilities::erase_features(const vk::PhysicalDeviceFeatures& features)
    -> void
{
    m_features.erase_features(features);
}

auto PhysicalDeviceCapabilities::erase_features(
    const vk::PhysicalDeviceVulkan11Features& features
) -> void
{
    m_features.erase_features(features);
}

auto PhysicalDeviceCapabilities::erase_features(
    const vk::PhysicalDeviceVulkan12Features& features
) -> void
{
    m_features.erase_features(features);
}

auto PhysicalDeviceCapabilities::erase_features(
    const vk::PhysicalDeviceVulkan13Features& features
) -> void
{
    m_features.erase_features(features);
}

auto PhysicalDeviceCapabilities::erase_features(
    const vk::PhysicalDeviceVulkan14Features& features
) -> void
{
    m_features.erase_features(features);
}

auto PhysicalDeviceCapabilities::erase(const PhysicalDeviceCapabilities& other) -> void
{
    PRECOND(version() >= other.version());
    for (const util::StringLiteral extension : other.extensions())
    {
        erase_extension(extension);
    }
    m_features.erase_features(other.features_chain().root());
}

auto PhysicalDeviceCapabilities::upgrade_to_Vulkan11() -> void
{
    PRECOND(m_version < vk::ApiVersion11);

    m_version = vk::ApiVersion11;

    vk::PhysicalDeviceVulkan11Features vulkan11_features{};
    std::erase_if(
        m_extension_names,
        [&vulkan11_features](const util::StringLiteral extension_name) -> bool
        {
            return try_promote_extension_to_vulkan11(
                extension_name,
                vulkan11_features
            );   //
        }
    );
    m_features.erase_and_merge_features_to_vulkan1x_feature_struct(vulkan11_features);
    m_features.merge(vulkan11_features);
}

auto PhysicalDeviceCapabilities::upgrade_to_Vulkan12() -> void
{
    PRECOND(m_version < vk::ApiVersion12);

    if (m_version < vk::ApiVersion11)
    {
        upgrade_to_Vulkan11();
    }

    m_version = vk::ApiVersion12;

    vk::PhysicalDeviceVulkan12Features vulkan12_features{};
    std::erase_if(
        m_extension_names,
        [&vulkan12_features](const util::StringLiteral extension_name) -> bool
        {
            return try_promote_extension_to_vulkan12(
                extension_name,
                vulkan12_features
            );   //
        }
    );
    m_features.erase_and_merge_features_to_vulkan1x_feature_struct(vulkan12_features);
    m_features.merge(vulkan12_features);
}

auto PhysicalDeviceCapabilities::upgrade_to_Vulkan13() -> void
{
    PRECOND(m_version < vk::ApiVersion13);

    if (m_version < vk::ApiVersion12)
    {
        upgrade_to_Vulkan12();
    }

    m_version = vk::ApiVersion13;

    vk::PhysicalDeviceVulkan13Features vulkan13_features{};
    std::erase_if(
        m_extension_names,
        [&vulkan13_features](const util::StringLiteral extension_name) -> bool
        {
            return try_promote_extension_to_vulkan13(
                extension_name,
                vulkan13_features
            );   //
        }
    );
    m_features.erase_and_merge_features_to_vulkan1x_feature_struct(vulkan13_features);
    m_features.merge(vulkan13_features);
}

auto PhysicalDeviceCapabilities::upgrade_to_Vulkan14() -> void
{
    PRECOND(m_version < vk::ApiVersion14);

    if (m_version < vk::ApiVersion13)
    {
        upgrade_to_Vulkan13();
    }

    m_version = vk::ApiVersion14;

    vk::PhysicalDeviceVulkan14Features vulkan14_features{};
    std::erase_if(
        m_extension_names,
        [&vulkan14_features](const util::StringLiteral extension_name) -> bool
        {
            return try_promote_extension_to_vulkan14(
                extension_name,
                vulkan14_features
            );   //
        }
    );
    m_features.erase_and_merge_features_to_vulkan1x_feature_struct(vulkan14_features);
    m_features.merge(vulkan14_features);
}

}   // namespace kiln::gfx::vulkan
