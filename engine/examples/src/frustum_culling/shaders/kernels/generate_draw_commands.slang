[require(spirv_1_6)]

import DrawCommand;
import DrawCommandGenerationPushConstants;


[SpecializationConstant]
const uint32_t MIN_SUBGROUP_SIZE = 32;

static const uint32_t INVOCATION_COUNT = 256;
static const uint32_t MAX_WORKGROUP_SIZE = INVOCATION_COUNT / MIN_SUBGROUP_SIZE;


groupshared uint32_t subgroup_sums[MAX_WORKGROUP_SIZE];
groupshared uint32_t subgroup_offsets[MAX_WORKGROUP_SIZE];
groupshared uint32_t workgroup_offset;


uint32_t global_prefix_sum(
    uint32_t  local_invocation_index,
    uint32_t  subgroup_index,
    uint32_t  subgroup_count,
    uint32_t  thread_value,
    uint32_t* global_counter,
)
{
    let subgroup_sum = WaveActiveSum(thread_value);
    let thread_offset  = WavePrefixSum(thread_value);
    if (WaveGetLaneIndex() == 0)
    {
        subgroup_sums[subgroup_index] = subgroup_sum;
    }
    GroupMemoryBarrierWithGroupSync();
    if (local_invocation_index == 0)
    {
        var workgroup_sum: uint32_t = 0;
        for (var i = 0; i < subgroup_count; ++i)
        {
            subgroup_offsets[i] = workgroup_sum;
            workgroup_sum += subgroup_sums[i];
        }
        InterlockedAdd(*global_counter, workgroup_sum, workgroup_offset);
    }
    GroupMemoryBarrierWithGroupSync();
    return workgroup_offset
            + subgroup_offsets[subgroup_index]
            + thread_offset;
}


[shader("compute")]
[require(spvGroupNonUniform, spvGroupNonUniformArithmetic)]
[numthreads(INVOCATION_COUNT, 1, 1)]
void main(
    uint32_t3                                  global_invocation_id   : SV_DispatchThreadID,
    uint32_t3                                  group_id               : SV_GroupID,
    uint32_t                                   local_invocation_index : SV_GroupIndex,
    uniform DrawCommandGenerationPushConstants push_constants,
)
{
    let subgroup_size = WaveGetLaneCount();
    let workgroup_size = (INVOCATION_COUNT - 1) / subgroup_size + 1;
    let subgroup_index = local_invocation_index / subgroup_size;

    let valid_workgroup_size
        = min(
            (push_constants.draw_command_count - group_id.x * INVOCATION_COUNT - 1)
                / subgroup_size
                + 1,
            workgroup_size
          );

    let draw_command_index = global_invocation_id.x;
    if (draw_command_index < push_constants.draw_command_count)
    {
        let instance_count = push_constants.draw_command_instance_counts[draw_command_index];

        let global_instance_offset
            = global_prefix_sum(
                local_invocation_index,
                subgroup_index,
                valid_workgroup_size,
                instance_count,
                push_constants.instance_counter
            );
        push_constants.draw_command_instance_offsets[draw_command_index] = global_instance_offset;

        let generated_draw_command_index =
            global_prefix_sum(
                local_invocation_index,
                subgroup_index,
                valid_workgroup_size,
                min(instance_count, 1),
                push_constants.draw_command_counter
            );

        if (instance_count != 0)
        {
            let generated_draw_command_address
                = push_constants.generated_draw_commands_buffer_address + generated_draw_command_index;
            *generated_draw_command_address
                = push_constants.original_draw_commands_buffer_address[draw_command_index];
            generated_draw_command_address->first_instance = global_instance_offset;
            generated_draw_command_address->instance_count = instance_count;
        }
    }
}
