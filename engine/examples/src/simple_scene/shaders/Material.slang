#ifndef __SLANG__
#pragma once
#define EXPORT
#define EXPORT_MEMBER
#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#define float3 glm::vec3
#define float4 glm::vec4
#else
module Material;
#define EXPORT public
#define EXPORT_MEMBER public
#endif

EXPORT struct Material {
    EXPORT_MEMBER float4 pbrMetallicRoughnessBaseColorFactor;

    EXPORT_MEMBER uint32_t pbrMetallicRoughnessBaseColorTextureIndex;
    EXPORT_MEMBER uint32_t pbrMetallicRoughnessBaseColorTextureTexCoord;
    EXPORT_MEMBER float    pbrMetallicRoughnessMetallicFactor;
    EXPORT_MEMBER float    pbrMetallicRoughnessRoughnessFactor;

    EXPORT_MEMBER uint32_t pbrMetallicRoughnessMetallicRoughnessTextureIndex;
    EXPORT_MEMBER uint32_t pbrMetallicRoughnessMetallicRoughnessTextureTexCoord;
    EXPORT_MEMBER uint32_t normalTextureIndex;
    EXPORT_MEMBER uint32_t normalTextureTexCoord;

    EXPORT_MEMBER float    normalTextureScale;
    EXPORT_MEMBER uint32_t occlusionTextureIndex;
    EXPORT_MEMBER uint32_t occlusionTextureTexCoord;
    EXPORT_MEMBER float    occlusionTextureScale;
    EXPORT_MEMBER uint32_t emissiveTextureIndex;
    EXPORT_MEMBER uint32_t emissiveTextureTexCoord;
    EXPORT_MEMBER float3   emissiveFactor;
    EXPORT_MEMBER float    alphaCutoff;
};
