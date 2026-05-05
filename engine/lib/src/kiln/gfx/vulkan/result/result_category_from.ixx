module;

#include <utility>

export module kiln.gfx.vulkan.result.result_category_from;

import vulkan_hpp;

import kiln.gfx.vulkan.result.ResultCategory;

namespace kiln::gfx::vulkan {

export [[nodiscard]]
constexpr auto result_category_from(const vk::Result result) noexcept -> ResultCategory
{
    if (std::to_underlying(result) >= 0)
    {
        return ResultCategory::eSuccess;
    }

    switch (result)
    {
        case vk::Result::eErrorExtensionNotPresent:
        case vk::Result::eErrorFeatureNotPresent:
        case vk::Result::eErrorIncompatibleDriver:
        case vk::Result::eErrorTooManyObjects:
        case vk::Result::eErrorFormatNotSupported:
        case vk::Result::eErrorNativeWindowInUseKHR:
        case vk::Result::eErrorInvalidExternalHandle:
        case vk::Result::eErrorInvalidDeviceAddressEXT:
            // eErrorInvalidDeviceAddressEXT is same as eErrorInvalidOpaqueCaptureAddress
        case vk::Result::eErrorValidationFailedEXT:
        case vk::Result::eErrorImageUsageNotSupportedKHR:
        case vk::Result::eErrorVideoPictureLayoutNotSupportedKHR:
        case vk::Result::eErrorVideoProfileFormatNotSupportedKHR:
        case vk::Result::eErrorVideoProfileCodecNotSupportedKHR:
        case vk::Result::eErrorVideoStdVersionNotSupportedKHR:
        case vk::Result::eErrorInvalidVideoStdParametersKHR:
            return ResultCategory::ePreconditionViolationError;
        case vk::Result::eErrorUnknown:
            return ResultCategory::eInternalContractViolationError;
        default: break;
    }

    return ResultCategory::eRuntimeError;
}

}   // namespace kiln::gfx::vulkan
