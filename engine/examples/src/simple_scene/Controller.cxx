module;

#include <algorithm>
#include <numbers>

#include <glm/ext/vector_double2.hpp>
#include <glm/gtc/quaternion.hpp>

module examples.simple_scene.Controller;

import kiln.wsi.Position;

namespace demo {

[[nodiscard]]
auto dvec2_from(const kiln::wsi::Position2d& position) noexcept -> glm::dvec2
{
    return glm::dvec2{ position.x, position.y };
}

Controller::Controller(const kiln::wsi::WindowProxy& window) noexcept
    : m_last_cursor_position{ dvec2_from(window.cursor_position()) }
{
}

auto Controller::update(
    const kiln::wsi::CursorMovedEvent& cursor_moved_event,
    Camera&                            camera
) noexcept -> void
{
    if (!m_activated)
    {
        return;
    }

    constexpr static double sensitivity{ 0.005 };

    const glm::dvec2 offset{
        dvec2_from(cursor_moved_event.new_cursor_position) - m_last_cursor_position,
    };
    m_last_cursor_position = dvec2_from(cursor_moved_event.new_cursor_position);

    const glm::dvec3 forward{
        camera.orientation() * glm::dvec3{ 0, 0, -1 }
    };
    const double pitch{
        std::clamp(
            std::asin(forward.y) + offset.y * sensitivity,
            -(std::numbers::pi / 2 - 0.0001),
            std::numbers::pi / 2 - 0.0001
        ),
    };
    const double yaw{ std::atan2(forward.z, forward.x) + offset.x * sensitivity };

    glm::dvec3 target_forward{
        cos(yaw) * cos(pitch),
        sin(pitch),
        sin(yaw) * cos(pitch),
    };
    target_forward = glm::normalize(target_forward);

    const glm::dvec3 right{
        glm::normalize(glm::cross(target_forward, glm::dvec3{ 0, 1, 0 }))
    };
    const glm::dvec3 up{ glm::cross(right, target_forward) };

    camera.set_orientation(glm::quat_cast(glm::dmat3{ right, up, -target_forward }));
}

auto Controller::activate(const kiln::wsi::Position2d& cursor_position) noexcept -> void
{
    if (!m_activated)
    {
        m_last_cursor_position = dvec2_from(cursor_position);
        m_activated            = true;
    }
}

auto Controller::deactivate() noexcept -> void
{
    m_activated = false;
}

}   // namespace demo
