module;

#include <utility>

export module kiln.gfx.renderer.debug_messenger.DebugMessenger;

import vulkan_hpp;

namespace kiln::gfx::renderer {

export class DebugMessenger {
public:
    explicit DebugMessenger(vk::DebugUtilsMessengerEXT&& debug_messenger)
        : m_debug_messenger{ std::move(debug_messenger) }
    {
    }

private:
    vk::DebugUtilsMessengerEXT m_debug_messenger;
};

}   // namespace kiln::gfx::renderer
