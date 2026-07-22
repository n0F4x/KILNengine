export module kiln.gfx.vulkan.DebugMessenger;

import vulkan;

import kiln.reg.BuildDirector;
import kiln.reg.EntryTraits;

namespace kiln::gfx::vulkan {

export class DebugMessenger {
public:
    explicit DebugMessenger(vk::raii::DebugUtilsMessengerEXT&& debug_messenger);

private:
    vk::raii::DebugUtilsMessengerEXT m_debug_messenger;
};

}   // namespace kiln::gfx::vulkan

template <>
struct kiln::reg::EntryTraits<kiln::gfx::vulkan::DebugMessenger> {
    static auto describe_build(BuildDirector<gfx::vulkan::DebugMessenger>& build_director)
        -> void;
};
