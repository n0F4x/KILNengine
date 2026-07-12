module;

#include <cstdint>

export module kiln.gfx.vulkan.Instance;

import vulkan;

import kiln.reg.BuildableEntry;
import kiln.reg.BuildDirector;

namespace kiln::gfx::vulkan {

export class Instance;

auto describe_build(reg::BuildDirector<Instance>& build_director) -> void;

class Instance : public reg::BuildableEntry<Instance, describe_build> {
public:
    explicit Instance(uint32_t api_version, vk::raii::Instance&& instance);


    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::Instance&;
    [[nodiscard]]
    auto api_version() const noexcept -> uint32_t;

private:
    uint32_t           m_api_version;
    vk::raii::Instance m_instance;
};

}   // namespace kiln::gfx::vulkan
