module;

#include <cstdint>
#include <optional>
#include <vector>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.vulkan.Instance;

import vulkan_hpp;

import kiln.app.config.ConfigBuilder;
import kiln.app.registry.EntryBase;
import kiln.app.registry.EntryBuilderInterface;
import kiln.util.StringLiteral;

namespace kiln::gfx::vulkan {

namespace internal {

export class InstanceBuilder;

}   // namespace internal

export class Instance : public app::EntryBase  {
public:
    using Builder = internal::InstanceBuilder;


    explicit Instance(uint32_t api_version, vk::raii::Instance&& instance);


    [[nodiscard]]
    auto get() const noexcept -> const vk::raii::Instance&;
    [[nodiscard]]
    auto api_version() const noexcept -> uint32_t;

private:
    uint32_t           m_api_version;
    vk::raii::Instance m_instance;
};

namespace internal {

struct InstanceBuilderPrecondition {
    [[nodiscard]]
    consteval static auto minimum_version() noexcept -> uint32_t;

    [[nodiscard]]
    static auto check_version_support(const vk::raii::Context& context) -> bool;


    explicit InstanceBuilderPrecondition(const vk::raii::Context& context);
};

export class InstanceBuilder : InstanceBuilderPrecondition,
                               public app::EntryBuilderInterface   //
{
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

    [[nodiscard]]
    static auto create(const app::ConfigBuilder& config_builder) -> InstanceBuilder;

    InstanceBuilder(
        const CreateInfo& create_info,
        [[kiln_lifetimebound]]
        const vk::raii::Context& context
    );

    auto target_api_version(uint32_t api_version) -> void;
    auto require_minimum_version(uint32_t version) -> void;
    auto enable_layer(util::StringLiteral layer_name) -> void;
    auto enable_extension(util::StringLiteral extension_name) -> void;
    auto enable_extension_if_available(util::StringLiteral extension_name) -> bool;

    [[nodiscard]]
    auto build() const -> Instance;

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

}   // namespace internal

}   // namespace kiln::gfx::vulkan

namespace kiln::gfx::vulkan::internal {

consteval auto InstanceBuilderPrecondition::minimum_version() noexcept -> uint32_t
{
    return vk::ApiVersion11;
}

consteval auto InstanceBuilder::minimum_version() noexcept -> uint32_t
{
    return InstanceBuilderPrecondition::minimum_version();
}

}   // namespace kiln::gfx::vulkan::internal
