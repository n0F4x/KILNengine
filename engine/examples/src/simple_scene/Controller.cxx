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

    const glm::dvec2 cursor_position{ dvec2_from(cursor_moved_event.new_cursor_position) };
    const glm::dvec2 offset{
        (cursor_position - m_last_cursor_position) * sensitivity,
    };

    m_last_cursor_position = cursor_position;

    constexpr static double pitch_limit{ std::numbers::pi / 2 };
    m_pitch = std::clamp(m_pitch + offset.y, -pitch_limit, pitch_limit);
    m_yaw   = std::remainder(m_yaw + offset.x, std::numbers::pi * 2);

    const glm::dquat yaw{
        glm::angleAxis(m_yaw, glm::dvec3{ 0, -1, 0 }),
    };
    const glm::dquat pitch{
        glm::angleAxis(m_pitch, glm::dvec3{ std::cos(m_yaw), 0, std::sin(m_yaw) }),
    };
    camera.set_orientation(glm::normalize(pitch * yaw));
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
