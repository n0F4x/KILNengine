#ifndef __SLANG__
#pragma once
#define EXPORT
#define PUBLIC_MEMBER
#include <cstdint>
template<typename>
using Ptr = uint64_t;
template<typename>
using ImmutablePtr = uint64_t;
#else
module InstanceIndexGenerationPushConstants;
#define EXPORT public
#define PUBLIC_MEMBER public
#endif

EXPORT struct InstanceIndexGenerationPushConstants {
    PUBLIC_MEMBER uint32_t               global_instance_count;
    PUBLIC_MEMBER ImmutablePtr<uint32_t> instance_draw_command_offsets;
    PUBLIC_MEMBER ImmutablePtr<uint32_t> instance_draw_command_indices;
    PUBLIC_MEMBER ImmutablePtr<uint32_t> draw_command_instance_offsets;
    PUBLIC_MEMBER Ptr<uint32_t>          instance_indices;
};
