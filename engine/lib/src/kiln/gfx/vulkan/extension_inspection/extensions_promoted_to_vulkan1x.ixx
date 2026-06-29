module;

#include <array>
#include <span>

export module kiln.gfx.vulkan.extension_inspection.extensions_promoted_to_vulkan1x;

import vulkan;

import kiln.util.StringLiteral;

namespace kiln::gfx::vulkan {

export [[nodiscard]]
constexpr auto extensions_promoted_to_vulkan11() -> std::span<const util::StringLiteral>;

export [[nodiscard]]
constexpr auto extensions_promoted_to_vulkan12() -> std::span<const util::StringLiteral>;

export [[nodiscard]]
constexpr auto extensions_promoted_to_vulkan13() -> std::span<const util::StringLiteral>;

export [[nodiscard]]
constexpr auto extensions_promoted_to_vulkan14() -> std::span<const util::StringLiteral>;

export [[nodiscard]]
constexpr auto extensions_promoted_to_vulkan1x() -> std::span<const util::StringLiteral>;

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan {

constexpr auto extensions_promoted_to_vulkan11() -> std::span<const util::StringLiteral>
{
    constexpr static std::array result{
        util::StringLiteral{ vk::KHRMultiviewExtensionName },
        util::StringLiteral{ vk::KHRGetPhysicalDeviceProperties2ExtensionName },
        util::StringLiteral{ vk::KHRDeviceGroupExtensionName },
        util::StringLiteral{ vk::KHRShaderDrawParametersExtensionName },
        util::StringLiteral{ vk::KHRMaintenance1ExtensionName },
        util::StringLiteral{ vk::KHRDeviceGroupCreationExtensionName },
        util::StringLiteral{ vk::KHRExternalMemoryCapabilitiesExtensionName },
        util::StringLiteral{ vk::KHRExternalMemoryExtensionName },
        util::StringLiteral{ vk::KHRExternalSemaphoreCapabilitiesExtensionName },
        util::StringLiteral{ vk::KHRExternalSemaphoreExtensionName },
        util::StringLiteral{ vk::KHR16BitStorageExtensionName },
        util::StringLiteral{ vk::KHRDescriptorUpdateTemplateExtensionName },
        util::StringLiteral{ vk::KHRExternalFenceCapabilitiesExtensionName },
        util::StringLiteral{ vk::KHRExternalFenceExtensionName },
        util::StringLiteral{ vk::KHRMaintenance2ExtensionName },
        util::StringLiteral{ vk::KHRVariablePointersExtensionName },
        util::StringLiteral{ vk::KHRDedicatedAllocationExtensionName },
        util::StringLiteral{ vk::KHRStorageBufferStorageClassExtensionName },
        util::StringLiteral{ vk::KHRRelaxedBlockLayoutExtensionName },
        util::StringLiteral{ vk::KHRGetMemoryRequirements2ExtensionName },
        util::StringLiteral{ vk::KHRSamplerYcbcrConversionExtensionName },
        util::StringLiteral{ vk::KHRBindMemory2ExtensionName },
        util::StringLiteral{ vk::KHRMaintenance3ExtensionName },
    };

    return result;
}

constexpr auto extensions_promoted_to_vulkan12() -> std::span<const util::StringLiteral>
{
    constexpr static std::array result{
        util::StringLiteral{ vk::KHRSamplerMirrorClampToEdgeExtensionName },
        util::StringLiteral{ vk::KHRShaderFloat16Int8ExtensionName },
        util::StringLiteral{ vk::KHRImagelessFramebufferExtensionName },
        util::StringLiteral{ vk::KHRCreateRenderpass2ExtensionName },
        util::StringLiteral{ vk::EXTSamplerFilterMinmaxExtensionName },
        util::StringLiteral{ vk::KHRImageFormatListExtensionName },
        util::StringLiteral{ vk::EXTDescriptorIndexingExtensionName },
        util::StringLiteral{ vk::EXTShaderViewportIndexLayerExtensionName },
        util::StringLiteral{ vk::KHRDrawIndirectCountExtensionName },
        util::StringLiteral{ vk::KHRShaderSubgroupExtendedTypesExtensionName },
        util::StringLiteral{ vk::KHR8BitStorageExtensionName },
        util::StringLiteral{ vk::KHRShaderAtomicInt64ExtensionName },
        util::StringLiteral{ vk::KHRDriverPropertiesExtensionName },
        util::StringLiteral{ vk::KHRShaderFloatControlsExtensionName },
        util::StringLiteral{ vk::KHRDepthStencilResolveExtensionName },
        util::StringLiteral{ vk::KHRTimelineSemaphoreExtensionName },
        util::StringLiteral{ vk::KHRVulkanMemoryModelExtensionName },
        util::StringLiteral{ vk::EXTScalarBlockLayoutExtensionName },
        util::StringLiteral{ vk::KHRSpirv14ExtensionName },
        util::StringLiteral{ vk::KHRSeparateDepthStencilLayoutsExtensionName },
        util::StringLiteral{ vk::EXTSeparateStencilUsageExtensionName },
        util::StringLiteral{ vk::KHRUniformBufferStandardLayoutExtensionName },
        util::StringLiteral{ vk::KHRBufferDeviceAddressExtensionName },
        util::StringLiteral{ vk::EXTHostQueryResetExtensionName },
    };

    return result;
}

constexpr auto extensions_promoted_to_vulkan13() -> std::span<const util::StringLiteral>
{
    constexpr static std::array result{
        util::StringLiteral{ vk::KHRDynamicRenderingExtensionName },
        util::StringLiteral{ vk::EXTTextureCompressionAstcHdrExtensionName },
        util::StringLiteral{ vk::EXTInlineUniformBlockExtensionName },
        util::StringLiteral{ vk::EXTPipelineCreationFeedbackExtensionName },
        util::StringLiteral{ vk::KHRShaderTerminateInvocationExtensionName },
        util::StringLiteral{ vk::EXTSubgroupSizeControlExtensionName },
        util::StringLiteral{ vk::EXTToolingInfoExtensionName },
        util::StringLiteral{ vk::EXTExtendedDynamicStateExtensionName },
        util::StringLiteral{ vk::EXTShaderDemoteToHelperInvocationExtensionName },
        util::StringLiteral{ vk::KHRShaderIntegerDotProductExtensionName },
        util::StringLiteral{ vk::EXTTexelBufferAlignmentExtensionName },
        util::StringLiteral{ vk::KHRShaderNonSemanticInfoExtensionName },
        util::StringLiteral{ vk::EXTPrivateDataExtensionName },
        util::StringLiteral{ vk::EXTPipelineCreationCacheControlExtensionName },
        util::StringLiteral{ vk::KHRSynchronization2ExtensionName },
        util::StringLiteral{ vk::KHRZeroInitializeWorkgroupMemoryExtensionName },
        util::StringLiteral{ vk::EXTYcbcr2Plane444FormatsExtensionName },
        util::StringLiteral{ vk::EXTImageRobustnessExtensionName },
        util::StringLiteral{ vk::KHRCopyCommands2ExtensionName },
        util::StringLiteral{ vk::EXT4444FormatsExtensionName },
        util::StringLiteral{ vk::KHRFormatFeatureFlags2ExtensionName },
        util::StringLiteral{ vk::EXTExtendedDynamicState2ExtensionName },
        util::StringLiteral{ vk::KHRMaintenance4ExtensionName },
    };

    return result;
}

constexpr auto extensions_promoted_to_vulkan14() -> std::span<const util::StringLiteral>
{
    constexpr static std::array result{
        util::StringLiteral{ vk::EXTPipelineRobustnessExtensionName },
        util::StringLiteral{ vk::KHRPushDescriptorExtensionName },
        util::StringLiteral{ vk::KHRGlobalPriorityExtensionName },
        util::StringLiteral{ vk::KHRDynamicRenderingLocalReadExtensionName },
        util::StringLiteral{ vk::EXTHostImageCopyExtensionName },
        util::StringLiteral{ vk::KHRMapMemory2ExtensionName },
        util::StringLiteral{ vk::KHRShaderSubgroupRotateExtensionName },
        util::StringLiteral{ vk::EXTPipelineProtectedAccessExtensionName },
        util::StringLiteral{ vk::KHRMaintenance5ExtensionName },
        util::StringLiteral{ vk::KHRVertexAttributeDivisorExtensionName },
        util::StringLiteral{ vk::KHRLoadStoreOpNoneExtensionName },
        util::StringLiteral{ vk::KHRShaderFloatControls2ExtensionName },
        util::StringLiteral{ vk::KHRIndexTypeUint8ExtensionName },
        util::StringLiteral{ vk::KHRLineRasterizationExtensionName },
        util::StringLiteral{ vk::KHRShaderExpectAssumeExtensionName },
        util::StringLiteral{ vk::KHRMaintenance6ExtensionName },
    };

    return result;
}

constexpr auto extensions_promoted_to_vulkan1x() -> std::span<const util::StringLiteral>
{
    constexpr static std::array result{
        // 1.1
        util::StringLiteral{ vk::KHRMultiviewExtensionName },
        util::StringLiteral{ vk::KHRGetPhysicalDeviceProperties2ExtensionName },
        util::StringLiteral{ vk::KHRDeviceGroupExtensionName },
        util::StringLiteral{ vk::KHRShaderDrawParametersExtensionName },
        util::StringLiteral{ vk::KHRMaintenance1ExtensionName },
        util::StringLiteral{ vk::KHRDeviceGroupCreationExtensionName },
        util::StringLiteral{ vk::KHRExternalMemoryCapabilitiesExtensionName },
        util::StringLiteral{ vk::KHRExternalMemoryExtensionName },
        util::StringLiteral{ vk::KHRExternalSemaphoreCapabilitiesExtensionName },
        util::StringLiteral{ vk::KHRExternalSemaphoreExtensionName },
        util::StringLiteral{ vk::KHR16BitStorageExtensionName },
        util::StringLiteral{ vk::KHRDescriptorUpdateTemplateExtensionName },
        util::StringLiteral{ vk::KHRExternalFenceCapabilitiesExtensionName },
        util::StringLiteral{ vk::KHRExternalFenceExtensionName },
        util::StringLiteral{ vk::KHRMaintenance2ExtensionName },
        util::StringLiteral{ vk::KHRVariablePointersExtensionName },
        util::StringLiteral{ vk::KHRDedicatedAllocationExtensionName },
        util::StringLiteral{ vk::KHRStorageBufferStorageClassExtensionName },
        util::StringLiteral{ vk::KHRRelaxedBlockLayoutExtensionName },
        util::StringLiteral{ vk::KHRGetMemoryRequirements2ExtensionName },
        util::StringLiteral{ vk::KHRSamplerYcbcrConversionExtensionName },
        util::StringLiteral{ vk::KHRBindMemory2ExtensionName },
        util::StringLiteral{ vk::KHRMaintenance3ExtensionName },
        //
        // 1.2
        util::StringLiteral{ vk::KHRSamplerMirrorClampToEdgeExtensionName },
        util::StringLiteral{ vk::KHRShaderFloat16Int8ExtensionName },
        util::StringLiteral{ vk::KHRImagelessFramebufferExtensionName },
        util::StringLiteral{ vk::KHRCreateRenderpass2ExtensionName },
        util::StringLiteral{ vk::EXTSamplerFilterMinmaxExtensionName },
        util::StringLiteral{ vk::KHRImageFormatListExtensionName },
        util::StringLiteral{ vk::EXTDescriptorIndexingExtensionName },
        util::StringLiteral{ vk::EXTShaderViewportIndexLayerExtensionName },
        util::StringLiteral{ vk::KHRDrawIndirectCountExtensionName },
        util::StringLiteral{ vk::KHRShaderSubgroupExtendedTypesExtensionName },
        util::StringLiteral{ vk::KHR8BitStorageExtensionName },
        util::StringLiteral{ vk::KHRShaderAtomicInt64ExtensionName },
        util::StringLiteral{ vk::KHRDriverPropertiesExtensionName },
        util::StringLiteral{ vk::KHRShaderFloatControlsExtensionName },
        util::StringLiteral{ vk::KHRDepthStencilResolveExtensionName },
        util::StringLiteral{ vk::KHRTimelineSemaphoreExtensionName },
        util::StringLiteral{ vk::KHRVulkanMemoryModelExtensionName },
        util::StringLiteral{ vk::EXTScalarBlockLayoutExtensionName },
        util::StringLiteral{ vk::KHRSpirv14ExtensionName },
        util::StringLiteral{ vk::KHRSeparateDepthStencilLayoutsExtensionName },
        util::StringLiteral{ vk::EXTSeparateStencilUsageExtensionName },
        util::StringLiteral{ vk::KHRUniformBufferStandardLayoutExtensionName },
        util::StringLiteral{ vk::KHRBufferDeviceAddressExtensionName },
        util::StringLiteral{ vk::EXTHostQueryResetExtensionName },
        //
        // 1.3
        util::StringLiteral{ vk::KHRDynamicRenderingExtensionName },
        util::StringLiteral{ vk::EXTTextureCompressionAstcHdrExtensionName },
        util::StringLiteral{ vk::EXTInlineUniformBlockExtensionName },
        util::StringLiteral{ vk::EXTPipelineCreationFeedbackExtensionName },
        util::StringLiteral{ vk::KHRShaderTerminateInvocationExtensionName },
        util::StringLiteral{ vk::EXTSubgroupSizeControlExtensionName },
        util::StringLiteral{ vk::EXTToolingInfoExtensionName },
        util::StringLiteral{ vk::EXTExtendedDynamicStateExtensionName },
        util::StringLiteral{ vk::EXTShaderDemoteToHelperInvocationExtensionName },
        util::StringLiteral{ vk::KHRShaderIntegerDotProductExtensionName },
        util::StringLiteral{ vk::EXTTexelBufferAlignmentExtensionName },
        util::StringLiteral{ vk::KHRShaderNonSemanticInfoExtensionName },
        util::StringLiteral{ vk::EXTPrivateDataExtensionName },
        util::StringLiteral{ vk::EXTPipelineCreationCacheControlExtensionName },
        util::StringLiteral{ vk::KHRSynchronization2ExtensionName },
        util::StringLiteral{ vk::KHRZeroInitializeWorkgroupMemoryExtensionName },
        util::StringLiteral{ vk::EXTYcbcr2Plane444FormatsExtensionName },
        util::StringLiteral{ vk::EXTImageRobustnessExtensionName },
        util::StringLiteral{ vk::KHRCopyCommands2ExtensionName },
        util::StringLiteral{ vk::EXT4444FormatsExtensionName },
        util::StringLiteral{ vk::KHRFormatFeatureFlags2ExtensionName },
        util::StringLiteral{ vk::EXTExtendedDynamicState2ExtensionName },
        util::StringLiteral{ vk::KHRMaintenance4ExtensionName },
        //
        // 1.4
        util::StringLiteral{ vk::EXTPipelineRobustnessExtensionName },
        util::StringLiteral{ vk::KHRPushDescriptorExtensionName },
        util::StringLiteral{ vk::KHRGlobalPriorityExtensionName },
        util::StringLiteral{ vk::KHRDynamicRenderingLocalReadExtensionName },
        util::StringLiteral{ vk::EXTHostImageCopyExtensionName },
        util::StringLiteral{ vk::KHRMapMemory2ExtensionName },
        util::StringLiteral{ vk::KHRShaderSubgroupRotateExtensionName },
        util::StringLiteral{ vk::EXTPipelineProtectedAccessExtensionName },
        util::StringLiteral{ vk::KHRMaintenance5ExtensionName },
        util::StringLiteral{ vk::KHRVertexAttributeDivisorExtensionName },
        util::StringLiteral{ vk::KHRLoadStoreOpNoneExtensionName },
        util::StringLiteral{ vk::KHRShaderFloatControls2ExtensionName },
        util::StringLiteral{ vk::KHRIndexTypeUint8ExtensionName },
        util::StringLiteral{ vk::KHRLineRasterizationExtensionName },
        util::StringLiteral{ vk::KHRShaderExpectAssumeExtensionName },
        util::StringLiteral{ vk::KHRMaintenance6ExtensionName },
    };

    return result;
}

}   // namespace kiln::gfx::vulkan
