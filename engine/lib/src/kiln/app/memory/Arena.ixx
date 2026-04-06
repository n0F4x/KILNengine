module;

#include <memory>
#include <memory_resource>
#include <utility>

export module kiln.app.memory.Arena;

import kiln.app.context.ContextBuilderInterface;

namespace kiln::app {

namespace internal {

export class ArenaBuilder;

}   // namespace internal

export class Arena {
public:
    using Builder = internal::ArenaBuilder;

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

namespace internal {

export class ArenaBuilder : public ContextBuilderInterface {
public:
    explicit ArenaBuilder() = default;

    explicit ArenaBuilder(Arena&& arena) noexcept : m_arena{ std::move(arena) } {}

    [[nodiscard]]
    // ReSharper disable once CppMemberFunctionMayBeConst
    auto arena() noexcept -> Arena&
    {
        return m_arena;
    }

    [[nodiscard]]
    auto build() && -> Arena
    {
        return std::move(m_arena);
    }

private:
    Arena m_arena;
};

}   // namespace internal

}   // namespace kiln::app
