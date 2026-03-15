module;

#include <format>
#include <ranges>
#include <sstream>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

module kiln.gfx.vulkan.default_debug_messenger_callback;

import kiln.config.engine_name;

namespace kiln::gfx::vulkan {

const auto logger{
    [] -> auto
    {
        auto result{
            spdlog::stdout_color_mt(std::format("{}:Vulkan", config::engine_name()))
        };
        result->set_level(spdlog::level::level_enum::debug);
        result->set_pattern("%^[%n](%r) %l:\n- %v%$");
        return result;
    }()   //
};

[[nodiscard]]
// ReSharper disable once CppNotAllPathsReturnValue
constexpr auto log_level_of(const vk::DebugUtilsMessageSeverityFlagBitsEXT severity)
    -> spdlog::level::level_enum
{
    switch (severity)
    {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            return spdlog::level::debug;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo: return spdlog::level::info;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            return spdlog::level::warn;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError: return spdlog::level::err;
    }
}

auto default_debug_messenger_callback(
    const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    const vk::DebugUtilsMessageTypeFlagsEXT,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* const
) -> vk::Bool32
{
    std::ostringstream message;

    message << std::format("{}\n", pCallbackData->pMessage);

    if (pCallbackData->queueLabelCount > 0)
    {
        message << "- Queue labels:\n";
        for (const char* label_name :
             std::span{ pCallbackData->pQueueLabels, pCallbackData->queueLabelCount }
                 | std::views::transform(&vk::DebugUtilsLabelEXT::pLabelName))
        {
            message << std::format("\t- \"{}\"\n", label_name);
        }
    }

    if (pCallbackData->cmdBufLabelCount > 0)
    {
        message << "- Command buffer labels:\n";
        for (const char* label_name :
             std::span{ pCallbackData->pCmdBufLabels, pCallbackData->cmdBufLabelCount }
                 | std::views::transform(&vk::DebugUtilsLabelEXT::pLabelName))
        {
            message << std::format("\t- \"{}\"\n", label_name);
        }
    }

    if (pCallbackData->objectCount > 0)
    {
        message << "- Objects:\n";
        for (const vk::DebugUtilsObjectNameInfoEXT& object_info :
             std::span{ pCallbackData->pObjects, pCallbackData->objectCount })
        {
            const char* const object_name{
                object_info.pObjectName != nullptr ? object_info.pObjectName : ""   //
            };
            message << std::format(
                "\t- {} ({}) {}\n",
                vk::to_string(object_info.objectType),
                object_info.objectHandle,
                object_name
            );
        }
    }

    logger->log(log_level_of(severity), message.str());

    return vk::False;
}

}   // namespace kiln::gfx::vulkan
