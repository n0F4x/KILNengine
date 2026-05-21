module;

#include <glm/vec3.hpp>

export module examples.simple_scene.AABB;

namespace demo {

export template <typename Repr_T = float>
struct AABB {
    glm::vec<3, Repr_T> min;
    glm::vec<3, Repr_T> max;
};

}   // namespace demo
