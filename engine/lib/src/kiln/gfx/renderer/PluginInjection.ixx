module;

#include <cassert>
#include <expected>
#include <span>

#include "kiln/util/contract_macros.hpp"

export module kiln.gfx.renderer.PluginInjection;

import kiln.gfx.renderer.Plugin;
import kiln.gfx.renderer.PluginFailedError;
import kiln.gfx.vulkan.InstancePlugin;
import kiln.util.contracts;
import kiln.util.Lazy;
import kiln.util.OptionalRef;
import kiln.util.StringLiteral;
import kiln.wsi.Plugin;
import kiln.wsi.vulkan_instance_extensions;

namespace kiln::gfx::renderer {

export class PluginInjection {
public:
    PluginInjection() = default;

    explicit PluginInjection(const bool headless) : m_headless{ headless } {}

    [[nodiscard]]
    auto operator()(
        vulkan::InstancePlugin&                    instance_plugin,
        const util::OptionalRef<const wsi::Plugin> wsi_plugin
    ) const -> Plugin
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
                                 throw PluginFailedError{
                                     "Vulkan surface creation is not supported"
                                 };
                             }   //
                         }
                     ))
            {
                {
                    [[maybe_unused]]
                    const bool success = instance_plugin->enable_extension(
                        util::StringLiteral::unsafe_create(extension_name)
                    );
                    assert(success);
                }
            }
        }

        return Plugin{ instance_plugin, m_headless };
    }

private:
    bool m_headless{};
};

}   // namespace kiln::gfx::renderer
