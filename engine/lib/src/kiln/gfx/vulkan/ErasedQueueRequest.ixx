export module kiln.gfx.vulkan.ErasedQueueRequest;

import kiln.gfx.vulkan.QueueRequestInterface;
import kiln.util.containers.Polymorphic;

namespace kiln::gfx::vulkan {

export using ErasedQueueRequest = util::Polymorphic<QueueRequestInterface>;

}   // namespace kiln::gfx::vulkan
