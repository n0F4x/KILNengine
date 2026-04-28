import Material;
import Primitive;
import Scene;

static const uint32_t UINT32_MAX = 0xFFFFFFFF;

struct VertexOutput {
    float4 color;
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

    uint32_t vertex_index = vertex_id;
    if (primitive.index_offset != UINT32_MAX) {
        vertex_index = scene.indices[primitive.index_offset + vertex_id];
    }

    float3 position = scene.positions[primitive.position_offset + vertex_index];
    if (scene.transforms != nullptr) {
        float4 world_position = mul(scene.transforms[0], float4(position, 1.0));
        position = divide(world_position.xyz, world_position.w);
    }

    float4 color = float4(0, 0, 0, 1);
    if (primitive.color_offset != UINT32_MAX) {
        color = *(float4*)(scene.vertices
                 + primitive.vertex_offset
                 + vertex_index * primitive.stride
                 + primitive.color_offset);
    }

    VertexOutput output;
    output.position = float4(position, 1.0);
    output.color = color;
    return output;
}

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    return input.color;
}
