module;

#include <numbers>

#include <glm/ext/vector_double2.hpp>

export module examples.simple_scene.Controller;

import kiln.wsi.event.events;
import kiln.wsi.Position;
import kiln.wsi.WindowProxy;

import examples.simple_scene.Camera;

namespace demo {

export class Controller {
public:
    explicit Controller(const kiln::wsi::WindowProxy& window) noexcept;

    auto update(
        const kiln::wsi::CursorMovedEvent& cursor_moved_event,
        Camera&                            camera
    ) noexcept -> void;

    auto activate(const kiln::wsi::Position2d& cursor_position) noexcept -> void;
    auto deactivate() noexcept -> void;

private:
    bool       m_activated{};
    glm::dvec2 m_last_cursor_position;
    double     m_pitch{};
    double     m_yaw{};
};

}   // namespace demo
