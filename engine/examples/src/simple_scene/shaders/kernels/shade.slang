import Camera;
import Scene;


struct VertexOutput {
    float4   clip_position : SV_Position;
    float3   world_position;
    float3   world_normal;
    float4   color;
};


[shader("vertex")]
VertexOutput main(
    uint32_t       draw_index  : SV_DrawIndex,
    uint32_t       vertex_id   : SV_VulkanVertexID,
    uint32_t       instance_id : SV_VulkanInstanceID,
    uniform Scene  scene,
    uniform Camera camera,
)
{
    let primitive = scene.primitive(draw_index);
    let material = scene.material(primitive);
    let instance_index = scene.instance_index(primitive, instance_id);
    let vertex_index = scene.vertex_index(primitive, vertex_id);
    let world_position = scene.world_position(primitive, instance_index, vertex_index);
    let world_normal = scene.world_normal(primitive, instance_index, vertex_index);
    let color = scene.vertex_color(primitive, vertex_index);

    VertexOutput output;
    output.world_position = world_position.xyz;
    output.clip_position = mul(camera.view_projection_matrix, world_position);
    output.world_normal = world_normal;
    // output.color = color * material.base_color_factor;
    output.color
        = float4(
            (draw_index >> 0) & 1,
            (draw_index >> 1) & 1,
            (draw_index >> 2) & 1,
            (draw_index >> 3) & 1,
          );
    return output;
}

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    float3 normal
        = all(input.world_normal == float3(0, 0, 0))
            ? normalize(cross(ddx(input.world_position), ddy(input.world_position)))
            : normalize(input.world_normal);

    // return float4(abs(normal), 1);
    return input.color;
}
