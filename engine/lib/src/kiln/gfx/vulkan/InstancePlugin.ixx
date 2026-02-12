module;

#include <cstdint>

#include "kiln/util/lifetime_bound.hpp"

export module kiln.gfx.vulkan.InstancePlugin;

import vulkan_hpp;

import kiln.app.App;
import kiln.config.Config;
import kiln.gfx.vulkan.context;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.util.StringLiteral;

namespace kiln::gfx::vulkan {

export class InstancePlugin {
public:
    [[nodiscard]]
    consteval static auto minimum_version() noexcept -> uint32_t;

    explicit InstancePlugin(
        const config::Config&                       config,
        [[lifetime_bound]] const vk::raii::Context& context = vulkan::context()
    );

    auto request_api_version(uint32_t api_version) -> void;
    [[nodiscard]]
    auto require_minimum_version(uint32_t version) -> bool;
    [[nodiscard]]
    auto enable_vulkan_layer(util::StringLiteral layer_name) -> bool;
    [[nodiscard]]
    auto enable_extension(util::StringLiteral extension_name) -> bool;

    auto operator()(app::App& app) const -> void
    {
        app.resources().insert(m_instance_builder.build());
    }

private:
    InstanceBuilder m_instance_builder;
};

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan {

consteval auto InstancePlugin::minimum_version() noexcept -> uint32_t
{
    return InstanceBuilder::minimum_version();
}

namespace internal {

[[nodiscard]]
auto builder_create_info(const config::Config& config) noexcept
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

}   // namespace internal

InstancePlugin::InstancePlugin(
    const config::Config&    config,
    const vk::raii::Context& context
)
    : m_instance_builder{ internal::builder_create_info(config), context }
{
}

auto InstancePlugin::request_api_version(const uint32_t api_version) -> void
{
    m_instance_builder.request_api_version(api_version);
}

auto InstancePlugin::require_minimum_version(const uint32_t version) -> bool
{
    return m_instance_builder.require_minimum_version(version);
}

auto InstancePlugin::enable_vulkan_layer(const util::StringLiteral layer_name) -> bool
{
    return m_instance_builder.enable_vulkan_layer(layer_name);
}

auto InstancePlugin::enable_extension(const util::StringLiteral extension_name) -> bool
{
    return m_instance_builder.enable_extension(extension_name);
}

}   // namespace kiln::gfx::vulkan
