#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <cstdint>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
using float4   = glm::vec4;
using float4x4 = glm::mat4x4;
#else
module Camera;
#define EXPORT public
#define PUBLIC_MEMBER public
#endif

EXPORT struct Camera {
    PUBLIC_MEMBER float4   position;
    PUBLIC_MEMBER float4x4 view_projection_matrix;
};
