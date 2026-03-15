export module kiln.gfx.renderer.debug_messenger.DebugMessenger;

import vulkan_hpp;

namespace kiln::gfx::renderer {

export class DebugMessenger {
public:

private:
    vk::DebugUtilsMessengerEXT m_debug_messenger;
};

}   // namespace kiln::gfx::renderer
