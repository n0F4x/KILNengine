module;

#include <cstdint>

module kiln.gfx.vulkan.InstancePlugin;

import vulkan_hpp;

namespace kiln::gfx::vulkan {

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

InstancePlugin::InstancePlugin(
    const config::Config&    config,
    const vk::raii::Context& context
)
    : m_instance_builder{ builder_create_info(config), context }
{
}

auto InstancePlugin::operator()() const -> vk::raii::Instance
{
    return m_instance_builder.build();
}

}   // namespace kiln::gfx::vulkan
