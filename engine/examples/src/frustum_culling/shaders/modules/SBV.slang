#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <glm/ext/vector_float3.hpp>
using float32_t  = float;
using float32_t3 = glm::vec3;
#else
module SBV;
#define EXPORT public
#define PUBLIC_MEMBER public
#endif

EXPORT struct SBV {
    PUBLIC_MEMBER float32_t3 center;
    PUBLIC_MEMBER float32_t  radius;
};
