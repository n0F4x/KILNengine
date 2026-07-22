module;

#include <algorithm>
#include <atomic>
#include <bit>
#include <cassert>
#include <expected>
#include <format>
#include <memory_resource>
#include <optional>
#include <ranges>
#include <utility>

#include "kiln/util/contract_macros.hpp"

module kiln.exec.data_structures.WorkTree;

import kiln.exec.data_structures.SignalTree;
import kiln.exec.data_structures.WorkContract;
import kiln.exec.data_structures.WorkContinuation;
import kiln.util.contracts;
import kiln.util.reflection;

namespace kiln::exec {

auto WorkContractSlot::assign(
    WorkContract&&                       work,
    std::optional<ReleaseWorkContract>&& release
) -> WorkContractSlot&
{
    m_work    = std::move(work);
    m_release = std::move(release);
    m_flags.store(WorkFlags::eNone);

    return *this;
}

// ReSharper disable once CppNotAllPathsReturnValue
auto WorkContractSlot::execute() -> WorkContinuation
{
    if (const bool should_be_released{
            (m_flags.load() & WorkFlags::eShouldBeReleased) != 0,
        };
        should_be_released)
    {
        release();
        return WorkContinuation::eRelease;
    }

    m_flags.fetch_and(WorkFlags::eAll - WorkFlags::eShouldBeScheduled);

    assert(m_work.has_value());
    // ReSharper disable once CppTooWideScope
    const WorkContinuation continuation = m_work->operator()();

    switch (continuation)
    {
        case WorkContinuation::eDontCare:
            if ((m_flags.fetch_and(WorkFlags::eAll - WorkFlags::eActive)
                 & WorkFlags::eShouldBeScheduled)
                != 0)
            {
                return WorkContinuation::eReschedule;
            }
            return WorkContinuation::eDontCare;
        case WorkContinuation::eReschedule:   //
            return WorkContinuation::eReschedule;
        case WorkContinuation::eRelease:
            if (m_release.has_value())
            {
                m_flags.fetch_or(WorkFlags::eShouldBeReleased);
                return WorkContinuation::eReschedule;
            }
            return WorkContinuation::eRelease;
    }
}

auto WorkContractSlot::schedule() -> bool
{
    return (m_flags.fetch_or(WorkFlags::eActive & WorkFlags::eShouldBeScheduled)
            & WorkFlags::eActive)
        == 0;
}

auto WorkContractSlot::schedule_release() -> WorkContinuation
{
    if (!m_release.has_value())
    {
        const bool should_be_released
            = (m_flags.fetch_or(WorkFlags::eActive & WorkFlags::eShouldBeReleased)
               & WorkFlags::eActive)
           == 0;
        if (should_be_released)
        {
            release();
            return WorkContinuation::eRelease;
        }
        return WorkContinuation::eDontCare;
    }

    m_flags.fetch_or(WorkFlags::eShouldBeReleased);
    return schedule() ? WorkContinuation::eReschedule : WorkContinuation::eDontCare;
}

auto WorkContractSlot::release() -> void
{
    if (m_release.has_value())
    {
        m_release->operator()();
    }
}

[[nodiscard]]
constexpr auto real_capacity_per_sub_tree(
    const uint64_t desired_capacity,
    const uint32_t number_of_threads
) -> uint64_t
{
    return std::max(
        desired_capacity / number_of_threads
            + std::min(desired_capacity % number_of_threads, uint64_t{ 1 }),
        static_cast<uint64_t>(SignalTree::minimum_number_of_levels())
    );
}

[[nodiscard]]
constexpr auto necessary_number_of_signal_tree_levels(
    const uint64_t number_of_work_contracts,
    const uint32_t number_of_threads
) -> uint32_t
{
    const auto capacity{
        real_capacity_per_sub_tree(number_of_work_contracts, number_of_threads)
    };

    return std::max(
        static_cast<uint32_t>(
            std::bit_width(capacity) + (std::has_single_bit(capacity) ? 0 : 1)
        ),
        SignalTree::minimum_number_of_levels()
    );
}

WorkTree::WorkTree(const uint64_t capacity, const uint32_t number_of_threads)
    : WorkTree{
          std::allocator_arg,
          std::pmr::get_default_resource(),
          capacity,
          number_of_threads,
      }
{
}

WorkTree::WorkTree(
    std::allocator_arg_t,
    const allocator_type& allocator,
    const uint64_t        capacity,
    const uint32_t        number_of_threads
)
    : m_free_signals{ allocator },
      m_contract_signals{ allocator },
      m_work_contracts_slots{
          real_capacity_per_sub_tree(capacity, number_of_threads) * number_of_threads,
          allocator,
      },
      m_per_thread_emplace_strategy_index{ number_of_threads, allocator },
      m_per_thread_execute_strategy_index{ number_of_threads, allocator }
{
    PRECOND(number_of_threads > 0);

    m_free_signals.reserve(number_of_threads);
    m_contract_signals.reserve(number_of_threads);
    for (auto _ : std::views::repeat(std::ignore, number_of_threads))
    {
        m_free_signals.emplace_back(
            necessary_number_of_signal_tree_levels(capacity, number_of_threads)
        );
        m_contract_signals.emplace_back(
            necessary_number_of_signal_tree_levels(capacity, number_of_threads)
        );
    }

    for (SignalTree& signal_tree : m_free_signals)
    {
        for (const uint32_t index : std::views::iota(0u, signal_tree.number_of_leaves()))
        {
            signal_tree.try_set_one(index);
        }
    }
}

auto WorkTree::get_allocator() const noexcept -> allocator_type
{
    return m_work_contracts_slots.get_allocator();
}

auto WorkTree::capacity() const noexcept -> uint64_t
{
    return m_work_contracts_slots.size();
}

auto WorkTree::optimized_for_thread_count() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(m_contract_signals.size());
}

auto WorkTree::try_emplace(WorkContract&& work) -> std::expected<WorkID, WorkContract>
{
    return try_emplace(std::move(work), std::nullopt)
        .transform_error(
            [](
                std::pair<WorkContract, std::optional<ReleaseWorkContract>>&& pair
            ) -> WorkContract { return std::move(pair.first); }
        );
}

auto WorkTree::try_emplace(WorkContract&& work, ReleaseWorkContract&& release)
    -> std::expected<WorkID, std::pair<WorkContract, ReleaseWorkContract>>
{
    return try_emplace(std::move(work), std::optional{ std::move(release) })
        .transform_error(
            [](
                std::pair<WorkContract, std::optional<ReleaseWorkContract>>&& pair
            ) -> std::pair<WorkContract, ReleaseWorkContract>
            {
                assert(pair.second.has_value());
                return std::make_pair(std::move(pair.first), std::move(*pair.second));
            }
        );
}

auto WorkTree::try_emplace_at(const WorkID work_id, WorkContract&& work)
    -> std::expected<void, WorkContract>
{
    return try_emplace_at(work_id, std::move(work), std::nullopt)
        .transform_error(
            [](
                std::pair<WorkContract, std::optional<ReleaseWorkContract>>&& pair
            ) -> WorkContract { return std::move(pair.first); }
        );
}

auto WorkTree::try_emplace_at(
    const WorkID          work_id,
    WorkContract&&        work,
    ReleaseWorkContract&& release
) -> std::expected<void, std::pair<WorkContract, ReleaseWorkContract>>
{
    return try_emplace_at(work_id, std::move(work), std::optional{ std::move(release) })
        .transform_error(
            [](
                std::pair<WorkContract, std::optional<ReleaseWorkContract>>&& pair
            ) -> std::pair<WorkContract, ReleaseWorkContract>
            {
                assert(pair.second.has_value());
                return std::make_pair(std::move(pair.first), std::move(*pair.second));
            }
        );
}

auto WorkTree::schedule(const WorkID work_id) -> void
{
    if (m_work_contracts_slots[work_id.underlying()].schedule())
    {
        m_contract_signals[work_id.underlying() % m_contract_signals.size()].try_set_one(
            static_cast<SignalTree::LeafIndex>(
                work_id.underlying() / m_contract_signals.size()
            )
        );
    }
}

auto WorkTree::schedule_for_release(const WorkID work_id) -> void
{
    const WorkContinuation work_continuation{
        m_work_contracts_slots[work_id.underlying()].schedule_release()
    };

    handle_work_result(work_id, work_continuation);
}

#define STRINGIFY(x) #x

[[nodiscard]]
constexpr auto make_fair_strategy(const uint32_t target_index) noexcept
{
    return [target_index, target_depth = std::bit_width(target_index + 1) - 1]   //
           [[nodiscard]]
           (const uint32_t current_index) -> TraversalBias
    {
        const auto current_depth{ std::bit_width(current_index + 1) - 1 };

        PRECOND(
            current_depth < target_depth,
            std::format("{} must not be leaf", STRINGIFY(current_index))
        );

        return ((target_index + 1) >> (target_depth - current_depth - 1)) & uint32_t{ 1 }
                 ? TraversalBias::eRight
                 : TraversalBias::eLeft;
    };
}

auto WorkTree::try_execute_one_work(const uint32_t thread_index) -> bool
{
    PRECOND(
        thread_index < m_contract_signals.size(),
        std::format(
            "`{}` must be smaller than the number of threads the `{}` instance is "
            "intended for.",
            STRINGIFY(thread_index),
            util::name_of<WorkTree>()
        )
    );

    for (const uint32_t offset : std::views::iota(0u, m_contract_signals.size()))
    {
        const uint32_t sub_tree_index{
            static_cast<uint32_t>((thread_index + offset) % m_contract_signals.size())
        };

        const std::optional<WorkID> work_id{
            m_contract_signals[sub_tree_index]
                .try_unset_one(
                    make_fair_strategy(
                        m_per_thread_execute_strategy_index[sub_tree_index].fetch_add(1)
                            % m_contract_signals[sub_tree_index].number_of_leaves()
                        + m_contract_signals[sub_tree_index].first_leaf_index()
                    )   //
                )
                .transform(
                    [this, sub_tree_index](const SignalTree::LeafIndex signal_index)
                    {
                        return signal_index * m_contract_signals.size()
                             + sub_tree_index;   //
                    }
                )
        };

        if (!work_id.has_value())
        {
            continue;
        }

        const WorkContinuation continuation
            = m_work_contracts_slots[work_id->underlying()].execute();

        handle_work_result(*work_id, continuation);

        return true;
    }

    return false;
}

auto WorkTree::try_emplace(
    WorkContract&&                       work,
    std::optional<ReleaseWorkContract>&& release
) -> std::expected<WorkID, std::pair<WorkContract, std::optional<ReleaseWorkContract>>>
{
    for (auto _ : std::views::repeat(std::ignore, m_free_signals.size()))
    {
        const auto sub_tree_index{
            m_next_available_sub_tree_index.fetch_add(1, std::memory_order::relaxed)
            % m_free_signals.size()
        };

        const std::optional<WorkID> work_id{
            m_free_signals[sub_tree_index]
                .try_unset_one(
                    make_fair_strategy(
                        m_per_thread_emplace_strategy_index[sub_tree_index].fetch_add(1)
                            % m_contract_signals[sub_tree_index].number_of_leaves()
                        + m_contract_signals[sub_tree_index].first_leaf_index()
                    )   //
                )
                .transform(
                    [this, sub_tree_index](const SignalTree::LeafIndex signal_index)
                    {
                        return signal_index * m_free_signals.size() + sub_tree_index;   //
                    }
                )
        };

        if (!work_id.has_value())
        {
            continue;
        }

        m_work_contracts_slots[work_id->underlying()]
            .assign(std::move(work), std::move(release));

        return *work_id;
    }

    return std::
        expected<WorkID, std::pair<WorkContract, std::optional<ReleaseWorkContract>>>{
            std::unexpect,
            std::move(work),
            std::move(release),
        };
}

auto WorkTree::try_emplace_at(
    const WorkID                         work_id,
    WorkContract&&                       work,
    std::optional<ReleaseWorkContract>&& release
) -> std::expected<void, std::pair<WorkContract, std::optional<ReleaseWorkContract>>>
{
    const auto sub_tree_index{ work_id.underlying() % m_free_signals.size() };

    const bool success = m_free_signals[sub_tree_index].try_unset_one_at(
        static_cast<SignalTree::LeafIndex>(work_id.underlying() / m_free_signals.size())
    );

    if (!success)
    {
        return std::
            expected<void, std::pair<WorkContract, std::optional<ReleaseWorkContract>>>{
                std::unexpect,
                std::move(work),
                std::move(release)
            };
    }

    m_work_contracts_slots[work_id.underlying()]
        .assign(std::move(work), std::move(release));

    return std::
        expected<void, std::pair<WorkContract, std::optional<ReleaseWorkContract>>>{};
}

auto WorkTree::handle_work_result(
    const WorkID           work_id,
    const WorkContinuation work_continuation
) -> void
{
    switch (work_continuation)
    {
        case WorkContinuation::eDontCare: break;
        case WorkContinuation::eReschedule:
            m_contract_signals[work_id.underlying() % m_contract_signals.size()]
                .try_set_one(
                    static_cast<SignalTree::LeafIndex>(
                        work_id.underlying() / m_contract_signals.size()
                    )
                );
            break;
        case WorkContinuation::eRelease:
            m_free_signals[work_id.underlying() % m_free_signals.size()].try_set_one(
                static_cast<SignalTree::LeafIndex>(
                    work_id.underlying() / m_free_signals.size()
                )
            );
            break;
    }
}

}   // namespace kiln::exec
