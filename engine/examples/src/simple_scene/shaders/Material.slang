#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#define float3 glm::vec3
#define float4 glm::vec4
#else
module Material;
#define EXPORT public
#define PUBLIC_MEMBER public
#endif

EXPORT struct Material {
    PUBLIC_MEMBER float4 pbrMetallicRoughnessBaseColorFactor;

    PUBLIC_MEMBER uint32_t pbrMetallicRoughnessBaseColorTextureIndex;
    PUBLIC_MEMBER uint32_t pbrMetallicRoughnessBaseColorTextureTexCoord;
    PUBLIC_MEMBER float    pbrMetallicRoughnessMetallicFactor;
    PUBLIC_MEMBER float    pbrMetallicRoughnessRoughnessFactor;

    PUBLIC_MEMBER uint32_t pbrMetallicRoughnessMetallicRoughnessTextureIndex;
    PUBLIC_MEMBER uint32_t pbrMetallicRoughnessMetallicRoughnessTextureTexCoord;
    PUBLIC_MEMBER uint32_t normalTextureIndex;
    PUBLIC_MEMBER uint32_t normalTextureTexCoord;

    PUBLIC_MEMBER float    normalTextureScale;
    PUBLIC_MEMBER uint32_t occlusionTextureIndex;
    PUBLIC_MEMBER uint32_t occlusionTextureTexCoord;
    PUBLIC_MEMBER float    occlusionTextureScale;
    PUBLIC_MEMBER uint32_t emissiveTextureIndex;
    PUBLIC_MEMBER uint32_t emissiveTextureTexCoord;
    PUBLIC_MEMBER float3   emissiveFactor;
    PUBLIC_MEMBER float    alphaCutoff;
};
