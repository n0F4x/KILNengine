#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <cstdint>
#include "Frustum.slang"
#include "SBV.slang"
template<typename>
using Ptr = uint64_t;
template<typename>
using ImmutablePtr = uint64_t;
#else
module FrustumCullingPushConstants;
#define EXPORT public
#define PUBLIC_MEMBER public
import Frustum;
import SBV;
#endif

EXPORT struct FrustumCullingPushConstants {
    PUBLIC_MEMBER uint32_t               global_instance_count;
    PUBLIC_MEMBER ImmutablePtr<uint32_t> instance_draw_command_indices;
    PUBLIC_MEMBER ImmutablePtr<SBV>      bounding_spheres;
    PUBLIC_MEMBER Frustum                frustum;
    PUBLIC_MEMBER Ptr<uint32_t>          draw_command_instance_counts;
    PUBLIC_MEMBER Ptr<uint32_t>          instance_draw_command_offsets;
};
