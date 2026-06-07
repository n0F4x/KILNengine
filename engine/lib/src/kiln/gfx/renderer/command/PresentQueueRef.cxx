module;

#include <variant>

#include "kiln/util/contract_macros.hpp"

module kiln.gfx.renderer.command.PresentQueueRef;

import vulkan_hpp;

import kiln.gfx.vulkan.result.check_result;
import kiln.gfx.vulkan.result.TypedResultCode;
import kiln.util.contracts;

namespace kiln::gfx::renderer {

PresentQueueRefPrecondition::PresentQueueRefPrecondition(const Queue& queue)
{
    PRECOND(queue.supports_presentation());
}

PresentQueueRef::PresentQueueRef(Queue& queue)
    : PresentQueueRefPrecondition{ queue },
      QueueRefBase{ queue }
{
}

auto PresentQueueRef::present(const vk::PresentInfoKHR& present_info) const
    -> std::variant<
        vulkan::TypedResultCode<vk::Result::eSuccess>,
        vulkan::TypedResultCode<vk::Result::eSuboptimalKHR>,
        vulkan::TypedResultCode<vk::Result::eErrorOutOfDateKHR>>
{
    return vulkan::check_result<vk::Result::eSuboptimalKHR, vk::Result::eErrorOutOfDateKHR>(
        // TODO: use C++ method when it handles out of date result
        get().get().getDispatcher()->vkQueuePresentKHR(
            *get().get(),
            reinterpret_cast<const vk::PresentInfoKHR::NativeType*>(&present_info)
        )
    );
}

}   // namespace kiln::gfx::renderer
