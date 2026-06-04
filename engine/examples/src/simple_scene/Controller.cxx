module;

#include <algorithm>
#include <chrono>
#include <numbers>

#include <glm/ext/vector_double2.hpp>
#include <glm/ext/vector_int2.hpp>
#include <glm/gtc/quaternion.hpp>

module examples.simple_scene.Controller;

import kiln.event.Timestamp;
import kiln.wsi.CursorMode;
import kiln.wsi.event.EventType;
import kiln.wsi.event.Key;
import kiln.wsi.Position;
import kiln.wsi.WindowProxy;

namespace demo {

[[nodiscard]]
auto dvec2_from(const kiln::wsi::Position2d& position) noexcept -> glm::dvec2
{
    return glm::dvec2{ position.x, position.y };
}

Controller::Controller(
    const kiln::wsi::WindowProxy& window,
    const double                  movement_speed
) noexcept
    : m_last_cursor_position{ dvec2_from(window.cursor_position()) },
      m_movement_speed{ movement_speed }
{
}

auto Controller::update(Camera& camera) const noexcept -> void
{
    camera.set_position(m_position);
    camera.set_orientation(orientation());
}

auto Controller::update(const kiln::event::Timestamp timestamp) noexcept -> void
{
    const auto delta_time{ timestamp - m_time };

    m_position += movement_orientation()
                * movement_direction()
                * std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
                      delta_time
                )
                      .count()
                * movement_sensitivity
                * m_movement_speed;

    m_time = timestamp;
}

auto Controller::update(
    const kiln::wsi::Event& event,
    kiln::event::Timestamp  timestamp,
    kiln::wsi::WindowProxy& window
) noexcept -> void
{
    timestamp = std::max(timestamp, m_time);

    if (event.type == kiln::wsi::EventType::eCursorMovedEvent)
    {
        update(event.cursor_moved_event, timestamp);
    }
    else if (event.type == kiln::wsi::EventType::eKeyPressedEvent)
    {
        update(event.key_pressed_event, timestamp, window);
    }
    else if (event.type == kiln::wsi::EventType::eKeyReleasedEvent)
    {
        update(event.key_released_event, timestamp, window);
    }
}

auto Controller::movement_direction() const noexcept -> glm::dvec3
{
    glm::ivec3 result{};

    if (m_movement_states.forward)
    {
        result.z += -1;
    }
    if (m_movement_states.backward)
    {
        result.z += 1;
    }
    if (m_movement_states.left)
    {
        result.x += -1;
    }
    if (m_movement_states.right)
    {
        result.x += 1;
    }
    if (m_movement_states.up)
    {
        result.y += -1;
    }
    if (m_movement_states.down)
    {
        result.y += 1;
    }

    if (result == glm::ivec3{})
    {
        return result;
    }
    return glm::normalize(glm::dvec3{ result });
}

auto Controller::movement_orientation() const noexcept -> glm::dquat
{
    return glm::angleAxis(m_yaw, glm::dvec3{ 0.0, -1.0, 0.0 });
}

auto Controller::orientation() const noexcept -> glm::dquat
{
    const glm::dquat yaw{
        glm::angleAxis(m_yaw, glm::dvec3{ 0, -1, 0 }),
    };
    const glm::dquat pitch{
        glm::angleAxis(m_pitch, glm::dvec3{ std::cos(m_yaw), 0, std::sin(m_yaw) }),
    };
    return glm::normalize(pitch * yaw);
}

auto Controller::update(
    const kiln::wsi::KeyPressedEvent& key_pressed_event,
    const kiln::event::Timestamp      timestamp,
    kiln::wsi::WindowProxy&           window
) noexcept -> void
{
    if (!m_cursor_disabled && key_pressed_event.key == kiln::wsi::Key::eLeftControl)
    {
        m_cursor_disabled = true;
        window.set_cursor_mode(kiln::wsi::CursorMode::eNormal);
    }

    update(timestamp);

    if (key_pressed_event.key == movement_keys.forward)
    {
        m_movement_states.forward = true;
    }
    if (key_pressed_event.key == movement_keys.backward)
    {
        m_movement_states.backward = true;
    }
    if (key_pressed_event.key == movement_keys.left)
    {
        m_movement_states.left = true;
    }
    if (key_pressed_event.key == movement_keys.right)
    {
        m_movement_states.right = true;
    }
    if (key_pressed_event.key == movement_keys.up)
    {
        m_movement_states.up = true;
    }
    if (key_pressed_event.key == movement_keys.down)
    {
        m_movement_states.down = true;
    }
}

auto Controller::update(
    const kiln::wsi::KeyReleasedEvent& key_released_event,
    const kiln::event::Timestamp       timestamp,
    kiln::wsi::WindowProxy&            window
) noexcept -> void
{
    if (m_cursor_disabled && key_released_event.key == kiln::wsi::Key::eLeftControl)
    {
        m_cursor_disabled = false;
        window.set_cursor_mode(kiln::wsi::CursorMode::eDisabledRaw);
    }

    update(timestamp);

    if (key_released_event.key == movement_keys.forward)
    {
        m_movement_states.forward = false;
    }
    if (key_released_event.key == movement_keys.backward)
    {
        m_movement_states.backward = false;
    }
    if (key_released_event.key == movement_keys.left)
    {
        m_movement_states.left = false;
    }
    if (key_released_event.key == movement_keys.right)
    {
        m_movement_states.right = false;
    }
    if (key_released_event.key == movement_keys.up)
    {
        m_movement_states.up = false;
    }
    if (key_released_event.key == movement_keys.down)
    {
        m_movement_states.down = false;
    }
}

auto Controller::update(
    const kiln::wsi::CursorMovedEvent& cursor_moved_event,
    const kiln::event::Timestamp       timestamp
) noexcept -> void
{
    const glm::dvec2 cursor_position{ dvec2_from(cursor_moved_event.new_cursor_position) };
    const glm::dvec2 rotation_offset{
        (cursor_position - m_last_cursor_position) * rotation_sensitivity,
    };
    m_last_cursor_position = cursor_position;

    if (!m_cursor_disabled)
    {
        update(timestamp);

        constexpr static double pitch_limit{ std::numbers::pi / 2 };
        m_pitch = std::clamp(m_pitch + rotation_offset.y, -pitch_limit, pitch_limit);
        m_yaw   = std::remainder(m_yaw + rotation_offset.x, std::numbers::pi * 2);
    }
}

}   // namespace demo
