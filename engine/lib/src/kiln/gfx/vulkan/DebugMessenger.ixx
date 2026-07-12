export module kiln.gfx.vulkan.DebugMessenger;

import vulkan;

import kiln.reg.BuildableEntry;
import kiln.reg.BuildDirector;

namespace kiln::gfx::vulkan {

export class DebugMessenger;

auto describe_build(reg::BuildDirector<DebugMessenger>& build_director) -> void;

class DebugMessenger : public reg::BuildableEntry<DebugMessenger, describe_build> {
public:
    explicit DebugMessenger(vk::raii::DebugUtilsMessengerEXT&& debug_messenger);

private:
    vk::raii::DebugUtilsMessengerEXT m_debug_messenger;
};

}   // namespace kiln::gfx::vulkan
