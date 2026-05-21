#ifndef __SLANG__
#pragma once
#define EXPORT
#define SLANG_PRIVATE_MEMBER
#include <cstdint>
template<typename>
using Ptr = uint64_t;
#else
module Scene;
#define EXPORT public
#define SLANG_PRIVATE_MEMBER private
import Material;
import DrawCommand;
import Index;
import Primitive;
static const uint32_t UINT32_MAX = 0xFFFFFFFF;
#endif

EXPORT struct Scene {
    SLANG_PRIVATE_MEMBER Ptr<uint8_t>     geometry_buffer_address;
    SLANG_PRIVATE_MEMBER Ptr<uint8_t>     material_buffer_address;
    SLANG_PRIVATE_MEMBER Ptr<uint8_t>     instance_buffer_address;
    SLANG_PRIVATE_MEMBER Ptr<DrawCommand> draw_command_buffer_address;

#ifdef __SLANG__
    public Primitive primitive(uint32_t draw_id)
    {
        return draw_command_buffer_address[draw_id].primitive;
    }

    public Index vertex_index(Primitive primitive, uint32_t vertex_id)
    {
        if (primitive.index_buffer_byte_offset == UINT32_MAX) {
            return vertex_id;
        }
        Index* index_buffer = (Index*)(geometry_buffer_address + primitive.index_buffer_byte_offset);
        return index_buffer[vertex_id];
    }

    public float4 vertex_world_position(
        uint32_t draw_id,
        uint32_t instance_id,
        Primitive primitive,
        Index vertex_index
    )
    {
        float3* position_buffer = (float3*)(geometry_buffer_address + primitive.position_buffer_byte_offset);
        return mul(transform(draw_id, instance_id), float4(position_buffer[vertex_index], 1.0));
    }

    public float3 vertex_world_normal(
         uint32_t draw_id,
         uint32_t instance_id,
         Primitive primitive,
         Index vertex_index
     )
    {
        if (primitive.normal_buffer_byte_offset == UINT32_MAX)
        {
            return float3(0.0f, 0.0f, 0.0f);
        }
        float3* normal_buffer = (float3*)(geometry_buffer_address + primitive.normal_buffer_byte_offset);
        return mul(normal_matrix(draw_id, instance_id), normal_buffer[vertex_index]);
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

    public Material material(Primitive primitive)
    {
        if (primitive.material_buffer_byte_offset == UINT32_MAX)
        {
            return Material();
        }
        Material* material_buffer
            = (Material*)(material_buffer_address + primitive.material_buffer_byte_offset);
        return *material_buffer;
    }

    private float4x4 transform(uint32_t draw_id, uint32_t instance_id)
    {
        float4x4* transform_buffer
            = (float4x4*)(
                instance_buffer_address
                + draw_command_buffer_address[draw_id].transform_buffer_byte_offset
              );

        return transform_buffer[instance_id];
    }

    private float3x3 normal_matrix(uint32_t draw_id, uint32_t instance_id)
    {
        float3x3* normal_matrix_buffer
            = (float3x3*)(
                instance_buffer_address
                + draw_command_buffer_address[draw_id].normal_matrix_buffer_byte_offset
              );

        return normal_matrix_buffer[instance_id];
    }
#endif
};
