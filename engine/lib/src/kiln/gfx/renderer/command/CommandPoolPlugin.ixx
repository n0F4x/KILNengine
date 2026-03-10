module;

#include <cstdint>

export module kiln.gfx.renderer.command.CommandPoolPlugin;

import kiln.app.App;
import kiln.gfx.renderer.command.CommandPool;
import kiln.gfx.renderer.device.Device;

namespace kiln::gfx::renderer {

export class CommandPoolPlugin {
public:
    explicit CommandPoolPlugin(const uint8_t number_of_frames)
        : m_number_of_frames{ number_of_frames }
    {
    }

    auto operator()(app::App& app) const -> void
    {
        app.context().insert(
            CommandPool{ app.context().at<Device>(), m_number_of_frames }
        );
    }

private:
    uint8_t m_number_of_frames;
};

}   // namespace kiln::gfx::renderer
