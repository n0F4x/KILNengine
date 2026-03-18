module;

#include <concepts>
#include <vector>

export module kiln.gfx.vulkan.ErasedQueueRequest;

import vulkan_hpp;

import kiln.gfx.vulkan.QueueFamilyIndex;
import kiln.gfx.vulkan.QueueFamilyInfo;
import kiln.gfx.vulkan.QueueRequestInterface;
import kiln.util.containers.Any;

namespace kiln::gfx::vulkan {

template <typename T>
struct ErasedQueueRequestPolicy {
    constexpr static bool value{ std::derived_from<T, QueueRequestInterface> };
};

template <typename ErasedQueueRequest_T>
class ErasedQueueRequestInterfaceMixin {
public:
    explicit ErasedQueueRequestInterfaceMixin(
        const util::AnyExtraVTableAccessor extra_vtable_accessor
    )
        : m_extra_vtable{ extra_vtable_accessor }
    {
    }

    [[nodiscard]]
    auto is_suitable(const vk::raii::PhysicalDevice& physical_device) const -> bool
    {
        return m_extra_vtable(static_cast<const ErasedQueueRequest_T&>(*this))
            .is_suitable(static_cast<const ErasedQueueRequest_T&>(*this), physical_device);
    }

    auto prepare_queue(
        std::vector<QueueFamilyInfo>&   current_selection,
        const vk::raii::PhysicalDevice& physical_device
    ) const -> void
    {
        m_extra_vtable(static_cast<const ErasedQueueRequest_T&>(*this))
            .prepare_queue(
                static_cast<const ErasedQueueRequest_T&>(*this),
                current_selection,
                physical_device
            );
    }

private:
    util::AnyExtraVTableAccessor m_extra_vtable;
};

template <typename ErasedQueueRequest_T>
struct ErasedQueueRequestExtraVTable {
    using IsSuitableFunc =
        auto(const ErasedQueueRequest_T&, const vk::raii::PhysicalDevice&) -> bool;
    using PrepareQueueFunc = auto(
        const ErasedQueueRequest_T&,
        std::vector<QueueFamilyInfo>&   current_selection,
        const vk::raii::PhysicalDevice& physical_device
    ) -> void;

    std::reference_wrapper<IsSuitableFunc>   is_suitable;
    std::reference_wrapper<PrepareQueueFunc> prepare_queue;

    template <typename QueueRequest_T>
    struct Operations {
        [[nodiscard]]
        static auto is_suitable(
            const ErasedQueueRequest_T&     erased_queue_request,
            const vk::raii::PhysicalDevice& physical_device
        ) -> bool
        {
            return util::any_cast<QueueRequest_T>(erased_queue_request)
                .is_suitable(physical_device);
        }

        static auto prepare_queue(
            const ErasedQueueRequest_T&     erased_queue_request,
            std::vector<QueueFamilyInfo>&   current_selection,
            const vk::raii::PhysicalDevice& physical_device
        ) -> void
        {
            util::any_cast<QueueRequest_T>(erased_queue_request)
                .prepare_queue(current_selection, physical_device);
        }

        constexpr static ErasedQueueRequestExtraVTable vtable{
            .is_suitable   = is_suitable,
            .prepare_queue = prepare_queue,
        };
    };
};

export using ErasedQueueRequest = util::BasicAny<util::DefaultAnyTraits<
    false,
    util::default_any_size(),
    util::default_any_alignment(),
    ErasedQueueRequestPolicy,
    ErasedQueueRequestInterfaceMixin,
    ErasedQueueRequestExtraVTable>>;

}   // namespace kiln::gfx::vulkan
