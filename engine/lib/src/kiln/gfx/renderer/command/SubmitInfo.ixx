export module kiln.gfx.renderer.command.SubmitInfo;

import vulkan_hpp;

import kiln.util.containers.OptionalRef;

namespace kiln::gfx::renderer {

export struct SubmitInfo {
    util::OptionalRef<const vk::raii::Fence> fence;
};

}   // namespace kiln::gfx::renderer
