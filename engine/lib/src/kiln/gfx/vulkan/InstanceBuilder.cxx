module;

#include <algorithm>
#include <print>
#include <ranges>
#include <vector>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.vulkan.InstanceBuilder;

import vulkan_hpp;

import kiln.gfx.vulkan.result.check_result;
import kiln.util.StringLiteral;
import kiln.util.contracts;

namespace kiln::gfx::vulkan {

auto InstanceBuilderPrecondition::check_version_support(const vk::raii::Context& context)
    -> bool
{
    return context.getDispatcher()->vkEnumerateInstanceVersion != nullptr
        && context.enumerateInstanceVersion() >= minimum_version();
}

InstanceBuilderPrecondition::InstanceBuilderPrecondition(
    [[maybe_unused]] const vk::raii::Context& context
)
{
    PRECOND(check_version_support(context));
}

auto InstanceBuilder::check_version_support(const vk::raii::Context& context) -> bool
{
    return InstanceBuilderPrecondition::check_version_support(context);
}

InstanceBuilder::InstanceBuilder(
    const CreateInfo&        create_info,
    const vk::raii::Context& context
)
    : InstanceBuilderPrecondition{ context },
      m_context{ context },
      m_application_name{ create_info.application_name },
      m_application_version{ create_info.application_version },
      m_engine_name{ create_info.engine_name },
      m_engine_version{ create_info.engine_version }
{
}

auto InstanceBuilder::request_api_version(const uint32_t api_version) -> void
{
    m_api_version = std::max(api_version, m_api_version);
}

auto InstanceBuilder::require_minimum_version(const uint32_t version) -> bool
{
    if (m_minimum_version >= version)
    {
        return true;
    }

    if (version > m_context.get().enumerateInstanceVersion())
    {
        return false;
    }

    m_minimum_version = version;

    return true;
}

auto InstanceBuilder::enable_vulkan_layer_if_available(
    const util::StringLiteral layer_name
) -> bool
{
    const std::vector<vk::LayerProperties> layer_properties{
        m_context.get().enumerateInstanceLayerProperties()
    };

    if (std::ranges::none_of(
            layer_properties,
            [layer_name](const char* const present_layer_name) -> bool
            {
                return layer_name
                    == util::StringLiteral::unsafe_create(present_layer_name);   //
            },
            &vk::LayerProperties::layerName
        ))
    {
        return false;
    }

    if (std::ranges::none_of(
            m_layer_names,
            [layer_name](const util::StringLiteral enabled_layer_name) -> bool
            {
                return layer_name == enabled_layer_name;   //
            }
        ))
    {
        m_layer_names.push_back(layer_name);
    }

    return true;
}

auto InstanceBuilder::enable_extension_if_available(
    const util::StringLiteral extension_name
) -> bool
{
    const std::vector<vk::ExtensionProperties> extension_properties{
        m_context.get().enumerateInstanceExtensionProperties()
    };

    if (std::ranges::none_of(
            extension_properties,
            [extension_name](const char* const supported_extension) -> bool
            {
                return extension_name
                    == util::StringLiteral::unsafe_create(supported_extension);   //
            },
            &vk::ExtensionProperties::extensionName
        ))
    {
        return false;
    }

    if (std::ranges::none_of(
            m_extension_names,
            [extension_name](const util::StringLiteral enabled_extension) -> bool
            {
                return extension_name == enabled_extension;   //
            }
        ))
    {
        m_extension_names.push_back(extension_name);
    }

    return true;
}

auto InstanceBuilder::build() const -> vk::raii::Instance
{
    const vk::ApplicationInfo application_info{
        .pApplicationName   = m_application_name.value_or(nullptr),
        .applicationVersion = m_application_version.value_or(0u),
        .pEngineName        = m_engine_name.value_or(nullptr),
        .engineVersion      = m_engine_version.value_or(0u),
        .apiVersion         = m_api_version,
    };

    const vk::InstanceCreateInfo instance_create_info{
        .pApplicationInfo        = &application_info,
        .enabledLayerCount       = static_cast<uint32_t>(m_layer_names.size()),
        .ppEnabledLayerNames     = m_layer_names.empty() ? nullptr
                                                         : m_layer_names.front().address(),
        .enabledExtensionCount   = static_cast<uint32_t>(m_extension_names.size()),
        .ppEnabledExtensionNames = m_extension_names.empty()
                                     ? nullptr
                                     : m_extension_names.front().address(),
    };

    return check_result(m_context.get().createInstance(instance_create_info));
}

}   // namespace kiln::gfx::vulkan
