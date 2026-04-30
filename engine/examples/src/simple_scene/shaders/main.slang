import Material;
import Primitive;
import Scene;

static const uint32_t UINT32_MAX = 0xFFFFFFFF;

struct VertexOutput {
    float4 position : SV_Position;
    float3 world_position;
    float3 normal;
    float4 color;
};

[shader("vertex")]
VertexOutput main(
    uint32_t vertex_id : SV_VertexID,
    uint32_t draw_id : SV_DrawIndex,
    uniform Scene scene
)
{
    Primitive primitive = scene.draw_commands[draw_id].primitive;

    uint32_t vertex_index = vertex_id;
    if (primitive.index_offset != UINT32_MAX) {
        vertex_index = scene.indices[primitive.index_offset + vertex_id];
    }

    float3 position = scene.positions[primitive.position_offset + vertex_index];

    float4 color = float4(0, 0, 0, 1);
    if (primitive.color_offset != UINT32_MAX) {
        color = *(float4*)(scene.vertices
                 + primitive.vertex_offset
                 + vertex_index * primitive.stride
                 + primitive.color_offset);
    }

    VertexOutput output;
    output.position = float4(position, 1.0);
    output.world_position = position;
    output.normal =
        primitive.normal_offset == UINT32_MAX
            ? float3(0, 0, 0)
            : *(float3*)(scene.vertices
                + primitive.vertex_offset
                + vertex_index * primitive.stride
                + primitive.normal_offset);
    output.color = color;
    return output;
}

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    float3 normal = input.normal;
    if (all(normal == float3(0, 0, 0))) {
        normal = normalize(cross(ddx(input.world_position), ddy(input.world_position)));
    }

    return float4(normal, 1);
}
