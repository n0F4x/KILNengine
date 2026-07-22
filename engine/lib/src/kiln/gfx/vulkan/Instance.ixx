module;

#include <cstdint>

export module kiln.gfx.vulkan.Instance;

import vulkan;

import kiln.reg.BuildDirector;
import kiln.reg.EntryTraits;

namespace kiln::gfx::vulkan {

export class Instance {
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

template <>
struct kiln::reg::EntryTraits<kiln::gfx::vulkan::Instance> {
    static auto describe_build(BuildDirector<gfx::vulkan::Instance>& build_director)
        -> void;
};
