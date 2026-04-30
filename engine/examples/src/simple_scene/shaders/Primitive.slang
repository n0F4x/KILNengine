#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <cstdint>
#else
module Primitive;
#define EXPORT public
#define PUBLIC_MEMBER public
static const uint32_t UINT32_MAX = 0xFFFFFFFF;
#endif

EXPORT struct Primitive {
    PUBLIC_MEMBER uint32_t index_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t position_offset;
    PUBLIC_MEMBER uint32_t vertex_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t stride = 0;
    PUBLIC_MEMBER uint32_t normal_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t tangent_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t uv_1_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t uv_2_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t color_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t material_index = UINT32_MAX;
};
