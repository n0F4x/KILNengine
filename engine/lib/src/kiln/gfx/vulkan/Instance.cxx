module;

#include <utility>

module kiln.gfx.vulkan.Instance;

import vulkan;

import kiln.reg.BuildDirector;
import kiln.gfx.vulkan.InstanceBuilder;

namespace kiln::gfx::vulkan {

Instance::Instance(const uint32_t api_version, vk::raii::Instance&& instance)
    : m_api_version{ api_version },
      m_instance{ std::move(instance) }
{
}

auto Instance::get() const noexcept -> const vk::raii::Instance&
{
    return m_instance;
}

auto Instance::api_version() const noexcept -> uint32_t
{
    return m_api_version;
}

}   // namespace kiln::gfx::vulkan

auto kiln::reg::EntryTraits<kiln::gfx::vulkan::Instance>::describe_build(
    BuildDirector<gfx::vulkan::Instance>& build_director
) -> void
{
    build_director.use_builder<gfx::vulkan::InstanceBuilder>();
}
