struct Primitive {
    uint32_t* indices;
    float3*   positions;
    float*    base_vertex_offset;
    uint      normal_offset;
    uint      tangent_offset;
    uint      uv_1_offset;
    uint      uv_2_offset;
    uint      color_offset;
    uint      material_index;
};

struct VertexOutput {
    float3 color;
    float4 position : SV_Position;
};

[shader("vertex")]
VertexOutput main(
    uint vertex_id : SV_VertexID,
    uint draw_id : SV_DrawIndex,
    uniform Primitive* primitives
)
{
    Primitive primitive = primitives[draw_id];
    uint32_t  index     = primitive.indices[vertex_id];
    float3    position  = primitive.positions[index];
    float3    color     = *(float3*)(primitive.base_vertex_offset + index + primitive.color_offset);

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
