module;

#include <cstdint>
#include <utility>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.vulkan.InstancePlugin;

import vulkan_hpp;

import kiln.app.plugin.PluginInterface;
import kiln.config.Config;
import kiln.gfx.vulkan.context;
import kiln.gfx.vulkan.InstanceBuilder;
import kiln.util.StringLiteral;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;

namespace kiln::gfx::vulkan {

export class InstancePlugin : public app::PluginInterface {
public:
    [[nodiscard]]
    consteval static auto minimum_version() noexcept -> uint32_t;

    explicit InstancePlugin(
        const config::Config&                           config,
        [[kiln_lifetimebound]] const vk::raii::Context& context = vulkan::context()
    );

    template <typename Self_T>
    [[nodiscard]]
    auto operator*(this Self_T&& self) -> util::forward_like_t<InstanceBuilder, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto operator->(this Self_T& self) -> util::const_like_t<InstanceBuilder, Self_T>*;

    [[nodiscard]]
    auto operator()() const -> vk::raii::Instance;

private:
    InstanceBuilder m_instance_builder;
};

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan {

consteval auto InstancePlugin::minimum_version() noexcept -> uint32_t
{
    return InstanceBuilder::minimum_version();
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

}   // namespace kiln::gfx::vulkan
