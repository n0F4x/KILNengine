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
module DrawCommandGenerationPushConstants;
#define EXPORT public
#define PUBLIC_MEMBER public
import DrawCommand;
#endif

EXPORT struct DrawCommandGenerationPushConstants {
    PUBLIC_MEMBER uint32_t                  draw_command_count;
    PUBLIC_MEMBER ImmutablePtr<DrawCommand> original_draw_commands_buffer_address;
    PUBLIC_MEMBER ImmutablePtr<uint32_t>    draw_command_instance_counts;
    PUBLIC_MEMBER Ptr<uint32_t>             instance_counter;
    PUBLIC_MEMBER Ptr<uint32_t>             draw_command_counter;
    PUBLIC_MEMBER Ptr<DrawCommand>          generated_draw_commands_buffer_address;
    PUBLIC_MEMBER Ptr<uint32_t>             draw_command_instance_offsets;
};
