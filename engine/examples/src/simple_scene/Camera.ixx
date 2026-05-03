module;

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_double.hpp>
#include <glm/ext/vector_double3.hpp>

export module examples.simple_scene.Camera;

namespace demo {

export class Camera {
public:
    explicit Camera(double aspect_ratio);

    [[nodiscard]]
    auto position() const noexcept -> const glm::dvec3&;
    [[nodiscard]]
    auto orientation() const noexcept -> const glm::dquat&;
    [[nodiscard]]
    auto fov() const noexcept -> double;
    [[nodiscard]]
    auto aspect_ratio() const noexcept -> double;
    [[nodiscard]]
    auto near_plane() const noexcept -> double;
    [[nodiscard]]
    auto far_plane() const noexcept -> double;

    auto set_position(const glm::dvec3& position) -> void;
    auto move(const glm::dvec3& amount) -> void;
    auto set_orientation(const glm::dquat& orientation) -> void;
    auto rotate(const glm::dquat& amount) -> void;
    auto set_fov(double fov) -> void;
    auto set_aspect_ratio(double aspect_ratio) -> void;
    auto set_near_plane(double near_plane) -> void;
    auto set_far_plane(double far_plane) -> void;

private:
    glm::dvec3 m_position{};
    glm::dquat m_orientation{ glm::identity<glm::dquat>() };
    double     m_fov{ glm::quarter_pi<double>() };
    double     m_aspect_ratio{};
    double     m_near_plane{ 0.01 };
    double     m_far_plane{ 1000.0 };
};

}   // namespace demo
