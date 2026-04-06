export module kiln.gfx.renderer.device.ErasedQueueRequest;

import kiln.gfx.renderer.device.QueueRequestInterface;
import kiln.util.containers.Polymorphic;

namespace kiln::gfx::renderer {

export using ErasedQueueRequest = util::Polymorphic<QueueRequestInterface>;

}   // namespace kiln::gfx::renderer
