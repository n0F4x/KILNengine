module;

#include <utility>

export module kiln.gfx.renderer.Plugin;

import vulkan_hpp;

import kiln.app.App;
import kiln.gfx.vulkan.DeviceBuilder;
import kiln.util.type_traits.const_like;
import kiln.util.type_traits.forward_like;

namespace kiln::gfx::renderer {

export class Plugin {
public:
    template <typename Self_T>
    [[nodiscard]]
    auto operator*(this Self_T&& self)
        -> util::forward_like_t<vulkan::DeviceBuilder, Self_T>
    {
        return std::forward_like<Self_T>(self.Plugin::m_device_builder);
    }

    template <typename Self_T>
    [[nodiscard]]
    auto operator->(this Self_T& self)
        -> util::const_like_t<vulkan::DeviceBuilder, Self_T>*
    {
        return &self.Plugin::m_device_builder;
    }

    auto operator()(app::App& app) const -> void
    {
        app.resources().insert(
            m_device_builder.build(app.resources().at<vk::raii::Instance>())
        );
    }

private:
    vulkan::DeviceBuilder m_device_builder;
};

}   // namespace kiln::gfx::renderer
