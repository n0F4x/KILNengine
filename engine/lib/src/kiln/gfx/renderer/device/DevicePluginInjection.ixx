module;

#include <cassert>
#include <expected>
#include <span>

#include "kiln/util/contract_macros.hpp"

export module kiln.gfx.renderer.device.DevicePluginInjection;

import kiln.gfx.renderer.device.DevicePlugin;
import kiln.gfx.renderer.device.DevicePluginFailedError;
import kiln.gfx.vulkan.InstancePlugin;
import kiln.util.contracts;
import kiln.util.Lazy;
import kiln.util.containers.OptionalRef;
import kiln.util.StringLiteral;
import kiln.wsi.Plugin;
import kiln.wsi.vulkan_instance_extensions;

namespace kiln::gfx::renderer {

export class DevicePluginInjection {
public:
    DevicePluginInjection() = default;

    explicit DevicePluginInjection(const bool headless) : m_headless{ headless } {}

    [[nodiscard]]
    auto operator()(
        vulkan::InstancePlugin&                    instance_plugin,
        const util::OptionalRef<const wsi::Plugin> wsi_plugin
    ) const -> DevicePlugin
    {
        if (!m_headless)
        {
            PRECOND(
                wsi_plugin.has_value(),
                "renderer plugin depends on non-existent wsi plugin"
            );

            for (const char* extension_name :
                 wsi::vulkan_instance_extensions(wsi_plugin->context())
                     .value_or(
                         util::Lazy{
                             [] -> std::span<const char* const>
                             {
                                 throw DevicePluginFailedError{
                                     "Vulkan surface creation is not supported"
                                 };
                             }   //
                         }
                     ))
            {
                {
                    [[maybe_unused]]
                    const bool success = instance_plugin->enable_extension_if_available(
                        util::StringLiteral::unsafe_create(extension_name)
                    );
                    assert(success);
                }
            }
        }

        return DevicePlugin{ instance_plugin, m_headless };
    }

private:
    bool m_headless{};
};

}   // namespace kiln::gfx::renderer
