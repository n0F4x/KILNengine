module;

#include <cstdint>
#include <optional>
#include <vector>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.vulkan.InstanceBuilder;

import vulkan_hpp;

import kiln.util.StringLiteral;

namespace kiln::gfx::vulkan {

struct InstanceBuilderPrecondition {
    [[nodiscard]]
    consteval static auto minimum_version() noexcept -> uint32_t
    {
        return vk::ApiVersion11;
    }

    [[nodiscard]]
    static auto check_version_support(const vk::raii::Context& context) -> bool;

    explicit InstanceBuilderPrecondition(const vk::raii::Context& context);
};

export class InstanceBuilder : InstanceBuilderPrecondition {
public:
    struct CreateInfo {
        std::optional<util::StringLiteral> engine_name;
        std::optional<uint32_t>            engine_version;
        std::optional<util::StringLiteral> application_name;
        std::optional<uint32_t>            application_version;
    };

    [[nodiscard]]
    consteval static auto minimum_version() noexcept -> uint32_t;

    [[nodiscard]]
    static auto check_version_support(const vk::raii::Context& context) -> bool;

    InstanceBuilder(
        const CreateInfo& create_info,
        [[kiln_lifetimebound]]
        const vk::raii::Context& context
    );

    auto request_api_version(uint32_t api_version) -> void;
    [[nodiscard]]
    auto require_minimum_version(uint32_t version) -> bool;
    auto enable_vulkan_layer_if_available(util::StringLiteral layer_name) -> bool;
    auto enable_extension_if_available(util::StringLiteral extension_name) -> bool;

    [[nodiscard]]
    auto build() const -> vk::raii::Instance;

private:
    std::reference_wrapper<const vk::raii::Context> m_context;
    std::optional<util::StringLiteral>              m_application_name;
    std::optional<uint32_t>                         m_application_version;
    std::optional<util::StringLiteral>              m_engine_name;
    std::optional<uint32_t>                         m_engine_version;
    uint32_t                                        m_api_version{ minimum_version() };
    uint32_t                         m_minimum_version{ minimum_version() };
    std::vector<util::StringLiteral> m_layer_names;
    std::vector<util::StringLiteral> m_extension_names;
};

consteval auto InstanceBuilder::minimum_version() noexcept -> uint32_t
{
    return InstanceBuilderPrecondition::minimum_version();
}

}   // namespace kiln::gfx::vulkan
