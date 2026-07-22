module;

#include <atomic>
#include <cstdint>
#include <memory_resource>
#include <optional>
#include <vector>

#include <tl/function_ref.hpp>

export module kiln.exec.data_structures.SignalTree;

namespace kiln::exec {

using NodeIndex = uint32_t;

using Counter = std::atomic<NodeIndex>;

static_assert(Counter::is_always_lock_free);

struct Node {
    Counter counter{};
};

export enum struct TraversalBias : std::uint8_t
{
    eLeft,
    eRight,
};

struct SignalTreePrecondition {
    explicit SignalTreePrecondition(uint32_t number_of_levels);
};

/**
 * Multi-producer multi-consumer
 */
export class SignalTree : private SignalTreePrecondition {
public:
    using allocator_type = std::pmr::polymorphic_allocator<>;
    using LeafIndex      = NodeIndex;
    using TraversalStrategy
        = tl::function_ref<TraversalBias(uint32_t current_node_index)>;


    [[nodiscard]]
    consteval static auto minimum_number_of_levels() noexcept -> uint32_t;


    SignalTree(SignalTree&&, const allocator_type&);

    explicit SignalTree(uint32_t number_of_levels);
    explicit SignalTree(
        std::allocator_arg_t,
        const allocator_type& allocator,
        uint32_t              number_of_levels
    );


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto full() const noexcept -> bool;

    [[nodiscard]]
    auto number_of_levels() const noexcept -> uint32_t;
    [[nodiscard]]
    auto number_of_leaves() const noexcept -> uint32_t;
    [[nodiscard]]
    auto number_of_branches() const noexcept -> uint32_t;
    [[nodiscard]]
    auto number_of_nodes() const noexcept -> uint32_t;
    [[nodiscard]]
    auto first_leaf_index() const noexcept -> uint32_t;


    auto try_set_one(LeafIndex leaf_index) -> bool;
    [[nodiscard]]
    auto try_unset_one(TraversalStrategy strategy) -> std::optional<LeafIndex>;
    [[nodiscard]]
    auto try_unset_one_at(LeafIndex leaf_index) -> bool;

private:
    std::pmr::vector<Node> m_nodes;

    auto set_branch(NodeIndex node_index) -> void;

    auto try_unset_one(NodeIndex node_index, TraversalStrategy strategy)
        -> std::optional<LeafIndex>;
    auto try_unset_one_at(NodeIndex node_index, NodeIndex target_index) -> bool;
    auto try_unset_leaf(NodeIndex node_index) -> std::optional<LeafIndex>;

    [[nodiscard]]
    auto is_leaf_index(NodeIndex node_index) const noexcept -> bool;
};

}   // namespace kiln::exec

namespace kiln::exec {

consteval auto SignalTree::minimum_number_of_levels() noexcept -> uint32_t
{
    return 2u;
}

}   // namespace kiln::exec
