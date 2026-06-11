module;

#include <cstdint>

export module kiln.gfx.vulkan.Instance;

import vulkan_hpp;

import kiln.app.registry.BuildableEntry;
import kiln.app.registry.EntryBuildDirector;

namespace kiln::gfx::vulkan {

export class Instance;

auto describe_build(app::EntryBuildDirector<Instance>& build_director) -> void;

class Instance : public app::BuildableEntry<Instance, describe_build> {
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
