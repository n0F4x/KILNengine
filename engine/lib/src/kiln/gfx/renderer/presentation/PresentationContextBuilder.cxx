module;

#include <span>

module kiln.gfx.renderer.presentation.PresentationContextBuilder;

import vulkan_hpp;

import kiln.gfx.renderer.presentation.PresentationContext;
import kiln.gfx.renderer.presentation.PresentationContextBuilderFailedError;
import kiln.util.Lazy;
import kiln.util.StringLiteral;
import kiln.wsi.vulkan_instance_extensions;

namespace kiln::gfx::renderer {

auto PresentationContextBuilder::create(
    const wsi::Context&      wsi_context,
    vulkan::InstanceBuilder& instance_builder,
    DeviceBuilder&           device_builder
) -> PresentationContextBuilder
{
    for (const char* extension_name :
         wsi::vulkan_instance_extensions(wsi_context)
             .value_or(
                 util::Lazy{
                     [] -> std::span<const char* const>
                     {
                         throw PresentationContextBuilderFailedError{
                             "Vulkan surface creation is not supported"
                         };
                     }   //
                 }
             ))
    {
        {
            instance_builder.enable_extension(
                util::StringLiteral::unsafe_create(extension_name)
            );
        }
    }

    device_builder.enable_extension(vk::KHRSwapchainExtensionName);

    return PresentationContextBuilder{};
}

auto PresentationContextBuilder::build() -> PresentationContext
{
    return PresentationContext{};
}

}   // namespace kiln::gfx::renderer
