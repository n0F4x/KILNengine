module;

#include <algorithm>
#include <memory_resource>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.vulkan.InstanceBuilder;

import kiln.app.config.Config;
import kiln.app.memory.MemoryArena;
import kiln.gfx.vulkan.context;
import kiln.gfx.vulkan.result.check_result;
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

[[nodiscard]]
auto builder_create_info(const app::Config& config) noexcept
    -> InstanceBuilder::CreateInfo
{
    return InstanceBuilder::CreateInfo{
        .engine_name    = config.engine_name(),
        .engine_version = vk::makeApiVersion(
            uint32_t{ 0 },
            config.engine_version().major,
            config.engine_version().minor,
            config.engine_version().patch
        ),
        .application_name    = config.app_name(),
        .application_version = vk::makeApiVersion(
            uint32_t{ 0 },
            config.app_version().major,
            config.app_version().minor,
            config.app_version().patch
        ),
    };
}

[[nodiscard]]
auto make_instance_builder(app::MemoryArena& memory_arena, const app::Config& config)
    -> InstanceBuilder
{
    return InstanceBuilder{
        std::allocator_arg,
        memory_arena.pool_allocator(),
        builder_create_info(config),
        context(),
    };
}

auto describe_build(app::BuildDirector<InstanceBuilder>& build_director) -> void
{
    build_director.use_function<make_instance_builder>();
}

auto InstanceBuilder::check_version_support(const vk::raii::Context& context) -> bool
{
    return InstanceBuilderPrecondition::check_version_support(context);
}

InstanceBuilder::InstanceBuilder(
    const InstanceBuilder& other,
    const allocator_type&  allocator
)
    : InstanceBuilderPrecondition{ other },
      m_context{ other.m_context },
      m_application_name{ other.m_application_name },
      m_application_version{ other.m_application_version },
      m_engine_name{ other.m_engine_name },
      m_engine_version{ other.m_engine_version },
      m_api_version{ other.m_api_version },
      m_minimum_version{ other.m_minimum_version },
      m_layer_names{ other.m_layer_names, allocator },
      m_extension_names{ other.m_extension_names, allocator }
{
}

InstanceBuilder::InstanceBuilder(InstanceBuilder&& other, const allocator_type& allocator)
    : InstanceBuilderPrecondition{ other },
      m_context{ other.m_context },
      m_application_name{ other.m_application_name },
      m_application_version{ other.m_application_version },
      m_engine_name{ other.m_engine_name },
      m_engine_version{ other.m_engine_version },
      m_api_version{ other.m_api_version },
      m_minimum_version{ other.m_minimum_version },
      m_layer_names{ std::move(other.m_layer_names), allocator },
      m_extension_names{ std::move(other.m_extension_names), allocator }
{
}

InstanceBuilder::InstanceBuilder(
    const CreateInfo&        create_info,
    const vk::raii::Context& context
)
    : InstanceBuilder{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          create_info,
          context,
      }
{
}

InstanceBuilder::InstanceBuilder(
    std::allocator_arg_t,
    const allocator_type&    allocator,
    const CreateInfo&        create_info,
    const vk::raii::Context& context
)
    : InstanceBuilderPrecondition{ context },
      m_context{ context },
      m_application_name{ create_info.application_name },
      m_application_version{ create_info.application_version },
      m_engine_name{ create_info.engine_name },
      m_engine_version{ create_info.engine_version },
      m_layer_names{ allocator },
      m_extension_names{ allocator }
{
}

auto InstanceBuilder::get_allocator() const noexcept -> allocator_type
{
    return m_layer_names.get_allocator();
}

auto InstanceBuilder::target_api_version(const uint32_t api_version) -> void
{
    m_api_version = std::max(api_version, m_api_version);
}

auto InstanceBuilder::require_minimum_version(const uint32_t version) -> void
{
    if (m_minimum_version >= version)
    {
        return;
    }

    PRECOND(version <= m_context.get().enumerateInstanceVersion());

    m_minimum_version = version;
}

auto InstanceBuilder::enable_layer(const util::StringLiteral layer_name) -> void
{
    PRECOND(
        std::ranges::any_of(
            m_context.get().enumerateInstanceLayerProperties(),
            [layer_name](const char* const present_layer_name) -> bool
            {
                return layer_name
                    == util::StringLiteral::unsafe_create(present_layer_name);   //
            },
            &vk::LayerProperties::layerName
        )
    );

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
}

auto InstanceBuilder::enable_extension(util::StringLiteral extension_name) -> void
{
    PRECOND(
        std::ranges::any_of(
            m_context.get().enumerateInstanceExtensionProperties(),
            [extension_name](const char* const supported_extension) -> bool
            {
                return extension_name
                    == util::StringLiteral::unsafe_create(supported_extension);   //
            },
            &vk::ExtensionProperties::extensionName
        )
    );

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
}

auto InstanceBuilder::enable_extension_if_available(
    const util::StringLiteral extension_name
) -> bool
{
    if (const std::vector<vk::ExtensionProperties> extension_properties{
            m_context.get().enumerateInstanceExtensionProperties() };
        std::ranges::none_of(
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

auto InstanceBuilder::build() const -> Instance
{
    const vk::ApplicationInfo application_info{
        .pApplicationName   = m_application_name.value_or(nullptr),
        .applicationVersion = m_application_version.value_or(0u),
        .pEngineName        = m_engine_name.value_or(nullptr),
        .engineVersion      = m_engine_version.value_or(0u),
        .apiVersion         = m_api_version,
    };

    const vk::InstanceCreateInfo instance_create_info{
        .pApplicationInfo      = &application_info,
        .enabledLayerCount     = static_cast<uint32_t>(m_layer_names.size()),
        .ppEnabledLayerNames   = m_layer_names.empty() ? nullptr
                                                       : m_layer_names.front().address(),
        .enabledExtensionCount = static_cast<uint32_t>(m_extension_names.size()),
        .ppEnabledExtensionNames
        = m_extension_names.empty() ? nullptr : m_extension_names.front().address(),
    };

    return Instance{
        application_info.apiVersion,
        check_result(m_context.get().createInstance(instance_create_info)),
    };
}

}   // namespace kiln::gfx::vulkan
