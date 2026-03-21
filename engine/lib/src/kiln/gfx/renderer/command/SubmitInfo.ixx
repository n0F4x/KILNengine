module;

#include <span>

export module kiln.gfx.renderer.command.SubmitInfo;

import vulkan_hpp;

import kiln.util.containers.OptionalRef;

namespace kiln::gfx::renderer {

export struct SubmitInfo {
    std::span<const vk::SemaphoreSubmitInfo> wait_semaphores;
    std::span<const vk::SemaphoreSubmitInfo> signal_semaphores;
    util::OptionalRef<const vk::raii::Fence> fence;
};

}   // namespace kiln::gfx::renderer
