import Camera;
import Scene;

struct VertexOutput {
    float4 clip_position : SV_Position;
    float3 world_position;
    float3 world_normal;
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
    let primitive = scene.primitive(draw_id);
    let vertex_index = scene.vertex_index(primitive, vertex_id);
    let world_position = scene.vertex_world_position(draw_id, instance_id, primitive, vertex_index);
    let world_normal = scene.vertex_world_normal(draw_id, instance_id, primitive, vertex_index);
    let color = scene.vertex_color(primitive, vertex_index);

    VertexOutput output;
    output.world_position = world_position.xyz;
    output.clip_position = mul(camera.view_projection_matrix, world_position);
    output.world_normal = world_normal;
    output.color = color;
    return output;
}

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    float3 normal
        = all(input.world_normal == float3(0, 0, 0))
            ? normalize(cross(ddx(input.world_position), ddy(input.world_position)))
            : normalize(input.world_normal);

    return float4(normal, 1);
}
