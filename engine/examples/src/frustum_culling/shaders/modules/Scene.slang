#ifndef __SLANG__
#pragma once
#define EXPORT
#define PRIVATE_MEMBER
#define NULLPTR uint64_t{}
#include <cstdint>
template<typename>
using Ptr = uint64_t;
#else
module Scene;
#define EXPORT public
#define PRIVATE_MEMBER private
#define NULLPTR nullptr
import Material;
import DrawCommand;
import Index;
import Primitive;
static const uint32_t UINT32_MAX = 0xFFFFFFFF;
#endif

EXPORT struct Scene {
    PRIVATE_MEMBER Ptr<uint8_t>     geometry_buffer_address;
    PRIVATE_MEMBER Ptr<uint8_t>     materials_buffer_address;
    PRIVATE_MEMBER Ptr<uint8_t>     instance_buffer_address;
    PRIVATE_MEMBER Ptr<uint32_t>    instance_indices_buffer_address = NULLPTR;
    PRIVATE_MEMBER Ptr<DrawCommand> draw_command_buffer_address;

#ifdef __SLANG__
    public Material material(Primitive primitive)
    {
        if (primitive.materials_buffer_byte_offset == UINT32_MAX)
        {
            return Material();
        }
        Material* materials_buffer
            = (Material*)(materials_buffer_address + primitive.materials_buffer_byte_offset);
        return *materials_buffer;
    }

    public Primitive primitive(uint32_t draw_index)
    {
        return draw_command_buffer_address[draw_index].primitive;
    }

    public uint32_t instance_index(Primitive primitive, uint32_t instance_index)
    {
        if (instance_indices_buffer_address != nullptr)
        {
            return instance_indices_buffer_address[instance_index] - primitive.global_instance_offset;
        }
        return instance_index;
    }

    public Index vertex_index(Primitive primitive, uint32_t vertex_id)
    {
        if (primitive.index_buffer_byte_offset == UINT32_MAX) {
            return vertex_id;
        }
        Index* index_buffer = (Index*)(geometry_buffer_address + primitive.index_buffer_byte_offset);
        return index_buffer[vertex_id];
    }

    public float4 world_position(
        Primitive primitive,
        uint32_t  instance_index,
        Index     vertex_index
    )
    {
        float3* position_buffer = (float3*)(geometry_buffer_address + primitive.position_buffer_byte_offset);
        return mul(transform(primitive, instance_index), float4(position_buffer[vertex_index], 1.0));
    }

    public float3 world_normal(
         Primitive primitive,
         uint32_t  instance_index,
         Index     vertex_index,
     )
    {
        if (primitive.normal_buffer_byte_offset == UINT32_MAX)
        {
            return float3(0.0f, 0.0f, 0.0f);
        }
        float3* normal_buffer = (float3*)(geometry_buffer_address + primitive.normal_buffer_byte_offset);
        return mul(normal_matrix(primitive, instance_index), normal_buffer[vertex_index]);
    }

    public float4 vertex_color(Primitive primitive, Index vertex_index)
    {
        if (primitive.color_buffer_byte_offset == UINT32_MAX)
        {
            return float4(1, 1, 1, 1);
        }
        float4* color_buffer = (float4*)(geometry_buffer_address + primitive.color_buffer_byte_offset);
        return color_buffer[vertex_index];
    }

    private float4x4 transform(Primitive primitive, uint32_t instance_index)
    {
        float4x4* transform_buffer
            = (float4x4*)(
                instance_buffer_address
                + primitive.transform_buffer_byte_offset
              );

        return transform_buffer[instance_index];
    }

    private float3x3 normal_matrix(Primitive primitive, uint32_t instance_index)
    {
        float3x3* normal_matrix_buffer
            = (float3x3*)(
                instance_buffer_address
                + primitive.normal_matrix_buffer_byte_offset
              );

        return normal_matrix_buffer[instance_index];
    }
#endif
};
