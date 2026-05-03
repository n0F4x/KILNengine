#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <cstdint>
#include "Primitive.slang"
#else
module DrawCommand;
import Primitive;
#define EXPORT public
#define PUBLIC_MEMBER public
#endif

EXPORT struct DrawCommand
{
    PUBLIC_MEMBER uint32_t  vertex_count;
    PUBLIC_MEMBER uint32_t  instance_count;
    PUBLIC_MEMBER uint32_t  first_vertex;
    PUBLIC_MEMBER uint32_t  first_instance;
    PUBLIC_MEMBER Primitive primitive;
    PUBLIC_MEMBER uint32_t  transform_byte_offset;
    PUBLIC_MEMBER uint32_t  normal_matrix_byte_offset;
};
