module;

#include <cstdint>
#include <memory_resource>
#include <optional>
#include <span>
#include <vector>

export module kiln.gfx.vulkan.QueueFamilyInfo;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.QueueInfo;
import kiln.gfx.vulkan.structure_chain.StructureChain;
import kiln.util.containers.OptionalRef;

namespace kiln::gfx::vulkan {

class DeviceCreateInfo {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;


    DeviceCreateInfo(const DeviceCreateInfo&, const allocator_type& allocator);
    DeviceCreateInfo(DeviceCreateInfo&&, const allocator_type& allocator);

    explicit DeviceCreateInfo(
        QueueFamilyIndex           family_index,
        vk::DeviceQueueCreateFlags flags = {}
    );
    explicit DeviceCreateInfo(
        std::allocator_arg_t,
        const allocator_type&      allocator,
        QueueFamilyIndex           family_index,
        vk::DeviceQueueCreateFlags flags = {}
    );

    explicit operator const vk::DeviceQueueCreateInfo&() const noexcept;

    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;
    [[nodiscard]]
    auto family_index() const noexcept -> QueueFamilyIndex;
    [[nodiscard]]
    auto flags() const noexcept -> vk::DeviceQueueCreateFlags;
    [[nodiscard]]
    auto queue_count() const noexcept -> uint32_t;

    auto emplace_back(float priority = 0.5f) -> void;

private:
    StructureChain<vk::DeviceQueueCreateInfo> m_info;
    std::pmr::vector<float>                   m_priorities;
};

export class QueueFamilyInfo {
public:
    using allocator_type   = std::pmr::polymorphic_allocator<>;
    using DeviceCreateInfo = DeviceCreateInfo;


    QueueFamilyInfo(const QueueFamilyInfo&, const allocator_type& allocator);
    QueueFamilyInfo(QueueFamilyInfo&&, const allocator_type& allocator);

    explicit QueueFamilyInfo(
        QueueFamilyIndex                  family_index,
        const vk::QueueFamilyProperties2& properties,
        bool                              supports_presentation
    );
    explicit QueueFamilyInfo(
        std::allocator_arg_t,
        const allocator_type&             allocator,
        QueueFamilyIndex                  family_index,
        const vk::QueueFamilyProperties2& properties,
        bool                              supports_presentation
    );

    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;
    [[nodiscard]]
    auto index() const noexcept -> QueueFamilyIndex;
    [[nodiscard]]
    auto queue_count() const noexcept -> uint32_t;
    [[nodiscard]]
    auto max_queue_count() const noexcept -> uint32_t;
    [[nodiscard]]
    auto flags() const noexcept -> vk::QueueFlags;
    [[nodiscard]]
    auto supports_presentation() const noexcept -> bool;
    [[nodiscard]]
    auto full() const noexcept -> bool;

    [[nodiscard]]
    auto try_create_info(vk::DeviceQueueCreateFlags flags) const noexcept
        -> util::OptionalRef<const DeviceCreateInfo>;
    [[nodiscard]]
    auto create_infos() const noexcept -> std::span<const DeviceCreateInfo>;


    auto try_emplace(vk::DeviceQueueCreateFlags flags = {}, float priority = 0.5f)
        -> std::optional<QueueInfo>;

private:
    QueueFamilyIndex                   m_family_index;
    uint32_t                           m_max_queue_count;
    vk::QueueFlags                     m_flags;
    bool                               m_supports_presentation;
    std::pmr::vector<DeviceCreateInfo> m_create_infos;


    auto try_emplace_create_info(vk::DeviceQueueCreateFlags flags = {})
        -> DeviceCreateInfo&;
};

}   // namespace kiln::gfx::vulkan
