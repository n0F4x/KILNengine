module;

#include <optional>

module kiln.gfx.renderer.command.QueueProvider;

namespace kiln::gfx::renderer {

QueueProvider::QueueProvider(const Queues& queues) : m_queues{ queues } {}

auto QueueProvider::graphics_queue() const -> std::optional<GraphicsQueueRef>
{
    return m_queues.graphics_queue_pack.transform(
        [](const QueuePack& queue_pack) -> GraphicsQueueRef
        {
            return GraphicsQueueRef{
                queue_pack.family_index,
                queue_pack.queue,
            };
        }
    );
}

}   // namespace kiln::gfx::renderer
