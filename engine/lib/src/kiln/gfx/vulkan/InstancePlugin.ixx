module;

#include <cstdint>
#include <utility>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.vulkan.InstancePlugin;

import vulkan_hpp;

import kiln.app.App;
import kiln.config.Config;
import kiln.gfx.vulkan.context;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.util.StringLiteral;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;

namespace kiln::gfx::vulkan {

export class InstancePlugin {
public:
    [[nodiscard]]
    consteval static auto minimum_version() noexcept -> uint32_t;

    explicit InstancePlugin(
        const config::Config&                       config,
        [[kiln_lifetimebound]] const vk::raii::Context& context = vulkan::context()
    );

    template <typename Self_T>
    [[nodiscard]]
    auto operator*(this Self_T&& self) -> util::forward_like_t<InstanceBuilder, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto operator->(this Self_T& self) -> util::const_like_t<InstanceBuilder, Self_T>*;

    auto operator()(app::App& app) const -> void;

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

template <typename Self_T>
auto InstancePlugin::operator*(this Self_T&& self)
    -> util::forward_like_t<InstanceBuilder, Self_T>
{
    return std::forward_like<Self_T>(self.m_instance_builder);
}

template <typename Self_T>
auto InstancePlugin::operator->(this Self_T& self)
    -> util::const_like_t<InstanceBuilder, Self_T>*
{
    return &self.m_instance_builder;
}

auto InstancePlugin::operator()(app::App& app) const -> void
{
    app.resources().insert(m_instance_builder.build());
}

}   // namespace kiln::gfx::vulkan
