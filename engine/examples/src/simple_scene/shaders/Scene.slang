#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
using float4x4 = glm::mat4x4;
template<typename>
using Ptr = uint64_t;
#else
module Scene;
#define EXPORT public
#define PUBLIC_MEMBER public
import Material;
import DrawCommand;
#endif

EXPORT struct Scene {
    PUBLIC_MEMBER Ptr<uint32_t>    indices;
    PUBLIC_MEMBER Ptr<float3>      positions;
    PUBLIC_MEMBER Ptr<float>       vertices;
    PUBLIC_MEMBER Ptr<Material>    materials;
    PUBLIC_MEMBER Ptr<float4x4>    transforms;
    PUBLIC_MEMBER Ptr<DrawCommand> draw_commands;
};
