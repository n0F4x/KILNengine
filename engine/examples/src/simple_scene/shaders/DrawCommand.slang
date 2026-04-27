#ifndef __SLANG__
#pragma once
#define EXPORT
#define EXPORT_MEMBER
#include <cstdint>
#include "Primitive.slang"
#else
module DrawCommand;
import Primitive;
#define EXPORT public
#define EXPORT_MEMBER public
#endif

EXPORT struct DrawCommand
{
    EXPORT_MEMBER uint32_t  index_count;
    EXPORT_MEMBER uint32_t  instance_count;
    EXPORT_MEMBER uint32_t  first_index;
    EXPORT_MEMBER int32_t   vertex_offset;
    EXPORT_MEMBER uint32_t  first_instance;
    EXPORT_MEMBER Primitive primitive;
};
