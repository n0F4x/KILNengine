module;

#include <optional>

#include <glm/common.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_double3.hpp>

#include <fastgltf/tools.hpp>

#include <kiln/util/contract_macros.hpp>

module examples.simple_scene.gltf_utils;

import kiln.util.contracts;

namespace demo {

[[nodiscard]]
auto vec3_from(const fastgltf::AccessorBoundsArray& position_accessor_bound) -> glm::vec3
{
    PRECOND(
        position_accessor_bound.type()
        == fastgltf::AccessorBoundsArray::BoundsType::float64
    );
    PRECOND(position_accessor_bound.size() == 3);

    return glm::vec3{
        position_accessor_bound.get<double>(0),
        position_accessor_bound.get<double>(1),
        position_accessor_bound.get<double>(2),
    };
}

[[nodiscard]]
auto change_basis(const glm::vec3& vector) -> glm::vec3
{
    return glm::vec3{
        vector.x,
        vector.y * -1,
        vector.z * -1,
    };
}

[[nodiscard]]
auto aabb_from(const fastgltf::Accessor& position_accessor) -> AABB<>
{
    PRECOND(position_accessor.min.has_value());
    PRECOND(position_accessor.max.has_value());

    return AABB{
        .min = change_basis(vec3_from(*position_accessor.min)),
        .max = change_basis(vec3_from(*position_accessor.max)),
    };
}

template <typename Repr_T>
[[nodiscard]]
auto operator*(const fastgltf::math::fmat4x4& transform, const AABB<Repr_T>& aabb)
    -> AABB<Repr_T>
{
    return AABB<Repr_T>{
        .min = *reinterpret_cast<const glm::mat4*>(transform.data())
             * glm::vec<4, Repr_T>{ aabb.min, 1 },
        .max = *reinterpret_cast<const glm::mat4*>(transform.data())
             * glm::vec<4, Repr_T>{ aabb.max, 1 },
    };
}

template <typename Repr_T>
[[nodiscard]]
auto merge(const AABB<Repr_T>& lhs, const AABB<Repr_T>& rhs) -> AABB<Repr_T>
{
    return AABB<Repr_T>{
        .min = glm::min(lhs.min, rhs.min),
        .max = glm::max(lhs.max, rhs.max),
    };
}

auto bounding_box_of(const fastgltf::Asset& model, const size_t scene_index)
    -> std::optional<AABB<>>
{
    std::optional<AABB<>> result{};

    fastgltf::iterateSceneNodes(
        model,
        scene_index,
        fastgltf::math::fmat4x4{},
        [&model, &result](
            const fastgltf::Node&          node,
            const fastgltf::math::fmat4x4& transform
        ) -> void
        {
            if (node.meshIndex.has_value())
            {
                for (const fastgltf::Primitive& primitive :
                     model.meshes[*node.meshIndex].primitives)
                {
                    const fastgltf::Attribute* position_attribute{
                        primitive.findAttribute("POSITION")
                    };
                    if (position_attribute == nullptr)
                    {
                        continue;
                    }

                    const fastgltf::Accessor& position_accessor{
                        model.accessors[position_attribute->accessorIndex]
                    };
                    if (!position_accessor.min.has_value())
                    {
                        throw std::runtime_error{
                            "Invalid glTF: POSITION accessor MUST have \"min\" defined"
                        };
                    }
                    if (!position_accessor.max.has_value())
                    {
                        throw std::runtime_error{
                            "Invalid glTF: POSITION accessor MUST have \"max\" defined"
                        };
                    }

                    if (!result.has_value())
                    {
                        result = transform * aabb_from(position_accessor);
                    }
                    else
                    {
                        result = merge(*result, transform * aabb_from(position_accessor));
                    }
                }
            }
        }
    );

    return result;
}

}   // namespace demo
