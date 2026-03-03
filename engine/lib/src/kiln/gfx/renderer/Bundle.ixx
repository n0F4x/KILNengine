module;

#include <cstdint>

export module kiln.gfx.renderer.Bundle;

import kiln.app.Builder;
import kiln.gfx.renderer.command.CommandPoolPluginInjection;
import kiln.gfx.renderer.device.DevicePluginInjection;
import kiln.gfx.renderer.memory.AllocatorPluginInjection;

namespace kiln::gfx::renderer {

export class Bundle {
public:
    struct CreateInfo {
        bool    headless{ true };
        uint8_t number_of_frames{ 3 };
    };

    Bundle() = default;
    explicit Bundle(const CreateInfo& create_info);

    auto operator()(app::Builder& builder) const -> void;

private:
    bool    m_headless;
    uint8_t m_number_of_frames;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

Bundle::Bundle(const CreateInfo& create_info)
    : m_headless{ create_info.headless },
      m_number_of_frames{ create_info.number_of_frames }
{
}

auto Bundle::operator()(app::Builder& builder) const -> void
{
    builder.inject_plugin(DevicePluginInjection{ m_headless });
    builder.inject_plugin(AllocatorPluginInjection{});
    builder.inject_plugin(CommandPoolPluginInjection{ m_number_of_frames });
}

}   // namespace kiln::gfx::renderer
