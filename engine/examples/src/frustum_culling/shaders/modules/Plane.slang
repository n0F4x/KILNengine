#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <glm/ext/vector_float3.hpp>
using float32_t  = float;
using float32_t3 = glm::vec3;
#else
module Plane;
#define EXPORT public
#define PUBLIC_MEMBER public
#endif

EXPORT struct Plane {
    PUBLIC_MEMBER float32_t3 normal;
    PUBLIC_MEMBER float32_t  offset;
};
