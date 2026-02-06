module;

#include <utility>

export module kiln.app.App;

import kiln.app.memory.Arena;
import kiln.app.resource.ResourceStack;
import kiln.util.type_traits.forward_like;

namespace kiln::app {

export class App {
public:
    template <typename Self_T>
    [[nodiscard]]
    auto arena(this Self_T&&) noexcept -> util::forward_like_t<Arena, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto resources(this Self_T&&) noexcept -> util::forward_like_t<ResourceStack, Self_T>;

private:
    Arena         m_arena;
    ResourceStack m_resources;
};

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T>
auto App::arena(this Self_T&& self) noexcept
    -> util::forward_like_t<Arena, Self_T>
{
    return std::forward_like<Self_T>(self.App::m_arena);
}

template <typename Self_T>
auto App::resources(this Self_T&& self) noexcept
    -> util::forward_like_t<ResourceStack, Self_T>
{
    return std::forward_like<Self_T>(self.App::m_resources);
}

}   // namespace kiln::app
