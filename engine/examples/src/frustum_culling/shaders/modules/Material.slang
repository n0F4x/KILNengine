#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <cstdint>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
using float3 = glm::vec3;
using float4 = glm::vec4;
#else
module Material;
#define EXPORT public
#define PUBLIC_MEMBER public
static const uint32_t UINT32_MAX = 0xFFFFFFFF;
#endif

EXPORT struct Material {
    PUBLIC_MEMBER float4   base_color_factor = float4(1, 1, 1, 1);
    PUBLIC_MEMBER uint32_t base_color_texture_index = UINT32_MAX;
    PUBLIC_MEMBER uint32_t base_color_texture_uv_index = UINT32_MAX;

    PUBLIC_MEMBER float    metallic_factor = 1;
    PUBLIC_MEMBER float    roughness_factor = 1;
    PUBLIC_MEMBER uint32_t metallic_roughness_texture_index = UINT32_MAX;
    PUBLIC_MEMBER uint32_t metallic_roughness_texture_uv_index = UINT32_MAX;

    PUBLIC_MEMBER uint32_t normal_texture_index = UINT32_MAX;
    PUBLIC_MEMBER uint32_t normal_texture_uv_index = UINT32_MAX;
    PUBLIC_MEMBER float    normal_texture_scale = 1;

    PUBLIC_MEMBER uint32_t occlusion_texture_index = UINT32_MAX;
    PUBLIC_MEMBER uint32_t occlusion_texture_uv_index = UINT32_MAX;
    PUBLIC_MEMBER float    occlusion_texture_strength = 1;

    PUBLIC_MEMBER uint32_t emissive_texture_index = UINT32_MAX;
    PUBLIC_MEMBER uint32_t emissive_texture_uv_index = UINT32_MAX;
    PUBLIC_MEMBER float3   emissive_factor = float3(0, 0, 0);

    PUBLIC_MEMBER float    alpha_cutoff = 0.5;
};
