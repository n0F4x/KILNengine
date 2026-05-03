module;

#include <glm/ext/quaternion_double.hpp>
#include <glm/ext/vector_double3.hpp>

module examples.simple_scene.Camera;

namespace demo {

Camera::Camera(const double aspect_ratio) : m_aspect_ratio{ aspect_ratio } {}

auto Camera::position() const noexcept -> const glm::dvec3&
{
    return m_position;
}

auto Camera::orientation() const noexcept -> const glm::dquat&
{
    return m_orientation;
}

auto Camera::fov() const noexcept -> double
{
    return m_fov;
}

auto Camera::aspect_ratio() const noexcept -> double
{
    return m_aspect_ratio;
}

auto Camera::near_plane() const noexcept -> double
{
    return m_near_plane;
}

auto Camera::far_plane() const noexcept -> double
{
    return m_far_plane;
}

auto Camera::set_position(const glm::dvec3& position) -> void
{
    m_position = position;
}

auto Camera::move(const glm::dvec3& amount) -> void
{
    m_position += amount;
}

auto Camera::set_orientation(const glm::dquat& orientation) -> void
{
    m_orientation = orientation;
}

auto Camera::rotate(const glm::dquat& amount) -> void
{
    m_orientation = glm::normalize(amount * m_orientation);
}

auto Camera::set_fov(const double fov) -> void
{
    m_fov = fov;
}

auto Camera::set_aspect_ratio(const double aspect_ratio) -> void
{
    m_aspect_ratio = aspect_ratio;
}

auto Camera::set_near_plane(const double near_plane) -> void
{
    m_near_plane = near_plane;
}

auto Camera::set_far_plane(const double far_plane) -> void
{
    m_far_plane = far_plane;
}

}   // namespace demo
