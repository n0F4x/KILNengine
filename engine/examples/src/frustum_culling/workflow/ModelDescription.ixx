module;

#include <functional>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <fastgltf/core.hpp>

export module examples.frustum_culling.workflow.ModelDescription;

namespace demo {

export struct ModelDescription {
    std::reference_wrapper<const fastgltf::Asset> model_asset;
    std::size_t                                   scene_index;
    glm::mat4x4 transform{ glm::identity<glm::mat4x4>() };
};

}   // namespace demo
