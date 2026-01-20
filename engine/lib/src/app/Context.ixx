module;

#include <cstdint>
#include <format>

#include <tsl/ordered_map.h>

#include <util/contract_macros.hpp>

export module kiln.app.Context;

import kiln.util.Any;
import kiln.util.contracts;
import kiln.util.concepts.naked;
import kiln.util.concepts.storable;
import kiln.util.OptionalRef;
import kiln.util.reflection;
import kiln.util.type_traits;

namespace kiln::app {

template <typename T>
concept item_c = util::storable_c<T> && util::naked_c<T>;

/*
 * References to contained items are valid until the context is alive.
 * Destroying a "moved-from" context does not invalidate any references to items.
 */
export class Context {
public:
    Context()                   = default;
    Context(const Context&)     = delete;
    Context(Context&&) noexcept = default;
    ~Context();

    auto operator=(const Context&) -> Context&     = delete;
    auto operator=(Context&&) noexcept -> Context& = default;

    template <item_c Item_T, typename... Args_T>
    auto emplace(Args_T&&... args) -> Item_T&;

    template <item_c Item_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::const_like_t<Item_T, Self_T>>;

    template <item_c Item_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::forward_like_t<Item_T, Self_T>;

    template <item_c Item_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    tsl::ordered_map<uint64_t, util::BasicMoveOnlyAny<0>> m_map;
};

}   // namespace kiln::app

namespace kiln::app {

template <item_c Item_T, typename... Args_T>
auto Context::emplace(Args_T&&... args) -> Item_T&
{
    return util::any_cast<Item_T>(
        m_map
            .try_emplace(
                util::hash<Item_T>(),
                std::in_place_type<Item_T>,
                std::forward<Args_T>(args)...
            )
            .first.value()   //
    );
}

template <item_c Item_T, typename Self_T>
auto Context::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::const_like_t<Item_T, Self_T>>
{
    const auto iter{ self.m_map.find(util::hash<Item_T>()) };
    if (iter == self.m_map.cend())
    {
        return std::nullopt;
    }

    return util::OptionalRef<util::const_like_t<Item_T, Self_T>>{
        util::any_cast<Item_T>(iter->second)
    };
}

template <item_c Item_T, typename Self_T>
auto Context::at(this Self_T&& self) -> util::forward_like_t<Item_T, Self_T>
{
    PRECOND(
        self.template contains<Item_T>(),
        std::format("Item {} not found", util::name_of<Item_T>())
    );

    return util::any_cast<Item_T>(
        std::forward_like<Self_T>(self.m_map.at(util::hash<Item_T>()))
    );
}

template <item_c Item_T>
auto Context::contains() const noexcept -> bool
{
    return m_map.contains(util::hash<Item_T>());
}

Context::~Context()
{
    while (!m_map.empty())
    {
        m_map.pop_back();
    }
}

}   // namespace kiln::app
