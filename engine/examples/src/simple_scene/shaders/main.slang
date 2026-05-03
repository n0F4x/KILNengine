import Camera;
import Index;
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
    uint32_t       draw_id : SV_DrawIndex,
    uint32_t       vertex_id : SV_VulkanVertexID,
    uint32_t       instance_id : SV_VulkanInstanceID,
    uniform Scene  scene,
    uniform Camera camera,
)
{
    Primitive primitive = scene.draw_command_buffer_address[draw_id].primitive;

    Index vertex_index = vertex_id;
    if (primitive.index_byte_offset != UINT32_MAX) {
        vertex_index = ((Index*)(scene.geometry_buffer_address + primitive.index_byte_offset))[vertex_id];
    }

    float3 position
        = ((float3*)(scene.geometry_buffer_address + primitive.position_byte_offset))[vertex_index];

    float4 color = float4(1, 1, 1, 1);
    if (primitive.color_byte_offset != UINT32_MAX) {
        color = ((float4*)(scene.geometry_buffer_address + primitive.color_byte_offset))[vertex_index];
    }

    float4x4 transform
        = ((float4x4*)(
            scene.instance_buffer_address + scene.draw_command_buffer_address[draw_id].transform_byte_offset
          ))[instance_id];
    float4 world_position = mul(transform, float4(position, 1.0));


    VertexOutput output;
    output.world_position = world_position.xyz;
    output.position = mul(camera.view_projection_matrix, world_position);
    output.normal =
        primitive.normal_byte_offset == UINT32_MAX
            ? float3(0, 0, 0)
            : mul(
                ((float3x3*)(
                    scene.instance_buffer_address
                        + scene.draw_command_buffer_address[draw_id].normal_matrix_byte_offset
                ))[instance_id],
                ((float3*)(scene.geometry_buffer_address + primitive.normal_byte_offset))[vertex_index]
              );
    output.color = color;
    return output;
}

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    float3 normal
        = all(input.normal == float3(0, 0, 0))
            ? normalize(cross(ddx(input.world_position), ddy(input.world_position)))
            : normalize(input.normal);

    return float4(abs(normal), 1);
}
