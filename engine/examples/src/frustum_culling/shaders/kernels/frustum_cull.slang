[require(spirv_1_6)]

import Frustum;
import FrustumCullingPushConstants;
import Plane;
import SBV;


static const uint32_t UINT32_MAX = 0xFFFFFFFF;

[SpecializationConstant]
const uint32_t MIN_SUBGROUP_SIZE = 32;


bool is_visible(SBV sphere, Frustum frustum)
{
    for (int i = 0; i < frustum.planes.getCount(); ++i)
    {
        float distance = dot(frustum.planes[i].normal, sphere.center) + frustum.planes[i].offset;
        if (distance < -sphere.radius)
        {
            return false;
        }
    }
    return true;
}

uint32_t get_component(uint4 value, uint32_t i)
{
    switch(i)
    {
        case 0:  return value.x;
        case 1:  return value.y;
        case 2:  return value.z;
        default: return value.w;
    }
}


[shader("compute")]
[require(spvGroupNonUniform, spvGroupNonUniformBallot, spvGroupNonUniformShuffle)]
[numthreads(MIN_SUBGROUP_SIZE, 1, 1)]
void main(
    uint32_t3                           global_invocation_id   : SV_DispatchThreadID,
    uint32_t                            local_invocation_index : SV_GroupIndex,
    uniform FrustumCullingPushConstants push_constants,
)
{
    let global_instance_index = global_invocation_id.x;

    let valid_instance = global_instance_index < push_constants.global_instance_count;
    if (valid_instance)
    {
        let visible
            = is_visible(push_constants.bounding_spheres[global_instance_index], push_constants.frustum);

        let draw_command_index
            = visible
                ? push_constants.instance_draw_command_indices[global_instance_index]
                : UINT32_MAX;

        var subgroup_draw_command_instance_count: uint32_t = 0;
        var thread_draw_command_instance_offset: uint32_t = 0;
        let thread_count = WaveActiveCountBits(valid_instance);
        for (var i = 0; i < thread_count; ++i)
        {
            let other_draw_command_index = WaveReadLaneAt(draw_command_index, i);

            if (draw_command_index == other_draw_command_index)
            {
                if (i < WaveGetLaneIndex())
                {
                    ++thread_draw_command_instance_offset;
                }
                ++subgroup_draw_command_instance_count;
            }
        }

        var subgroup_draw_command_instance_offset: uint32_t = 0;
        if (draw_command_index != UINT32_MAX && thread_draw_command_instance_offset == 0)
        {
            InterlockedAdd(
                push_constants.draw_command_instance_counts[draw_command_index],
                subgroup_draw_command_instance_count,
                subgroup_draw_command_instance_offset
            );
        }

        for (uint i = 0; i < WaveGetLaneCount(); ++i)
        {
           if (WaveReadLaneAt(thread_draw_command_instance_offset == 0, i)
                  && draw_command_index == WaveReadLaneAt(draw_command_index, i))
           {
               subgroup_draw_command_instance_offset = WaveReadLaneAt(subgroup_draw_command_instance_offset, i);
               break;
           }
        }


        let draw_command_instance_offset
            = subgroup_draw_command_instance_offset + thread_draw_command_instance_offset;

        push_constants.instance_draw_command_offsets[global_instance_index]
            = visible ? draw_command_instance_offset : UINT32_MAX;
    }
}
