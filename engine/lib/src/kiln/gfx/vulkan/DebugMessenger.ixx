export module kiln.gfx.vulkan.DebugMessenger;

import vulkan_hpp;

import kiln.app.registry.BuildableEntry;
import kiln.app.registry.BuildDirector;

namespace kiln::gfx::vulkan {

export class DebugMessenger;

auto describe_build(app::BuildDirector<DebugMessenger>& build_director) -> void;

class DebugMessenger
    : public app::BuildableEntry<DebugMessenger, describe_build> {
public:
    explicit DebugMessenger(vk::raii::DebugUtilsMessengerEXT&& debug_messenger);

private:
    vk::raii::DebugUtilsMessengerEXT m_debug_messenger;
};

}   // namespace kiln::gfx::vulkan
