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
    PUBLIC_MEMBER uint32_t index_byte_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t position_byte_offset;
    PUBLIC_MEMBER uint32_t normal_byte_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t tangent_byte_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t uv_0_byte_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t uv_1_byte_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t color_byte_offset = UINT32_MAX;
    PUBLIC_MEMBER uint32_t material_index = UINT32_MAX;
};
