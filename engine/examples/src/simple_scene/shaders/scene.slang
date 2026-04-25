import Material;

struct Primitive {
    uint32_t      material_index;
    uint32_t      index_offset;
    uint32_t      position_offset;
    uint32_t      vertex_offset;
    uint32_t      stride;
    uint32_t      normal_offset;
    uint32_t      tangent_offset;
    uint32_t      uv_1_offset;
    uint32_t      uv_2_offset;
    uint32_t      color_offset;
};

struct Scene {
    Primitive* primitives;
    Material*  materials;
    uint32_t*  indices;
    float3*    positions;
    float*     vertices;
};

struct VertexOutput {
    float3 color;
    float4 position : SV_Position;
};

[shader("vertex")]
VertexOutput main(
    uint32_t vertex_id : SV_VertexID,
    uint32_t draw_id : SV_DrawIndex,
    uniform Scene scene
)
{
    Primitive primitive = scene.primitives[draw_id];
    uint32_t  vertex_index     = scene.indices[vertex_id + primitive.index_offset];
    float3    position  = scene.positions[primitive.position_offset + vertex_index];
    float3    color =
        *(float3*)(scene.vertices
                    + primitive.vertex_offset
                    + vertex_index * primitive.stride
                    + primitive.color_offset);

    VertexOutput output;
    output.position = float4(position, 1.0);
    output.color = color;
    return output;
}

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    return float4(input.color, 1.0);
}
