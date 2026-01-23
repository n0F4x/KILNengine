#pragma once

#include <utility>

#include "kiln/util/GenericStack.hpp"
#include "kiln/util/type_traits/forward_like.hpp"

namespace kiln::app {

class App {
public:
    template <typename Self_T>
    [[nodiscard]]
    auto resources(this Self_T&&) noexcept
        -> util::forward_like_t<util::GenericStack, Self_T>;

private:
    util::GenericStack m_resources;
};

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T>
auto App::resources(this Self_T&& self) noexcept
    -> util::forward_like_t<util::GenericStack, Self_T>
{
    return std::forward_like<Self_T>(self.m_resources);
}

}   // namespace kiln::app
