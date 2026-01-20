module;

#include <utility>

export module kiln.app.App;

import kiln.app.Context;
import kiln.util.type_traits;

namespace kiln::app {

export class App {
public:
    template <typename Self_T>
    [[nodiscard]]
    auto context(this Self_T&&) noexcept -> util::forward_like_t<Context, Self_T>;

private:
    Context m_context;
};

}   // namespace kiln::app

namespace kiln::app {

template <typename Self_T>
auto App::context(this Self_T&& self) noexcept -> util::forward_like_t<Context, Self_T>
{
    return std::forward_like<Self_T>(self.m_context);
}

}   // namespace kiln::app
