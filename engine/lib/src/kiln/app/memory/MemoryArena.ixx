module;

#include <memory>
#include <memory_resource>
#include <utility>

export module kiln.app.memory.MemoryArena;

import kiln.app.context.ContextBuilderInterface;

namespace kiln::app {

export class MemoryArena {
public:
    class Builder;

    [[nodiscard]]
    // ReSharper disable once CppMemberFunctionMayBeConst
    auto pool_allocator() -> std::pmr::polymorphic_allocator<>
    {
        return m_pool_resource.get();
    }

    [[nodiscard]]
    // ReSharper disable once CppMemberFunctionMayBeConst
    auto make_transient_resource() -> std::pmr::monotonic_buffer_resource
    {
        return std::pmr::monotonic_buffer_resource{ m_transient_memory_resource.get() };
    }

private:
    std::shared_ptr<std::pmr::monotonic_buffer_resource> m_monotonic_resource{
        std::make_shared<std::pmr::monotonic_buffer_resource>()
    };
    std::shared_ptr<std::pmr::unsynchronized_pool_resource> m_pool_resource{
        std::make_shared<std::pmr::unsynchronized_pool_resource>(m_monotonic_resource.get())
    };
    std::shared_ptr<std::pmr::unsynchronized_pool_resource> m_transient_memory_resource{
        std::make_shared<std::pmr::unsynchronized_pool_resource>(m_monotonic_resource.get())
    };
};

class MemoryArena::Builder : public ContextBuilderInterface {
public:
    explicit Builder() = default;

    explicit Builder(MemoryArena&& memory_arena) noexcept : m_arena{ std::move(memory_arena) } {}

    [[nodiscard]]
    // ReSharper disable once CppMemberFunctionMayBeConst
    auto arena() noexcept -> MemoryArena&
    {
        return m_arena;
    }

    [[nodiscard]]
    auto build() && -> MemoryArena
    {
        return std::move(m_arena);
    }

private:
    MemoryArena m_arena;
};

}   // namespace kiln::app
