[require(spirv_1_6)]

import InstanceIndexGenerationPushConstants;


static const uint32_t UINT32_MAX = 0xFFFFFFFF;

static const uint32_t INVOCATION_COUNT = 256;


[shader("compute")]
[numthreads(INVOCATION_COUNT, 1, 1)]
void main(
    uint32_t3                                    global_invocation_id   : SV_DispatchThreadID,
    uniform InstanceIndexGenerationPushConstants push_constants,
)
{
    let global_instance_index = global_invocation_id.x;

    if (global_instance_index < push_constants.global_instance_count)
    {
        let instance_index = push_constants.instance_draw_command_offsets[global_instance_index];
        if (instance_index != UINT32_MAX)
        {
            let draw_command_index
                = push_constants.instance_draw_command_indices[global_instance_index];
            let draw_command_instance_offset
                = push_constants.draw_command_instance_offsets[draw_command_index];
            push_constants.instance_indices[draw_command_instance_offset + instance_index]
                = global_instance_index;
        }
    }
}
