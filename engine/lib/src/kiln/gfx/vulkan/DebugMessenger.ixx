export module kiln.gfx.vulkan.DebugMessenger;

import vulkan_hpp;

import kiln.app.context.ContextBuilderInterface;
import kiln.gfx.vulkan.Instance;
import kiln.gfx.vulkan.InstanceBuilder;

namespace kiln::gfx::vulkan {

class DebugMessengerBuilder;

export class DebugMessenger : public app::ContextBuilderInterface {
public:
    using Builder = DebugMessengerBuilder;


    explicit DebugMessenger(vk::raii::DebugUtilsMessengerEXT&& debug_messenger);

private:
    vk::raii::DebugUtilsMessengerEXT m_debug_messenger;
};

class DebugMessengerBuilder : public app::ContextBuilderInterface {
public:
    [[nodiscard]]
    static auto create(InstanceBuilder& instance_builder) -> DebugMessengerBuilder;

    [[nodiscard]]
    static auto build(const Instance& instance) -> DebugMessenger;
};

}   // namespace kiln::gfx::vulkan
