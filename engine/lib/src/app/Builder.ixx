module;

#include <utility>

export module kiln.app.Builder;

import kiln.app.App;
import kiln.app.ContextBuilder;

namespace kiln::app {

export class Builder {
public:
    [[nodiscard]]
    auto build() && -> App;

private:
    ContextBuilder m_resource_context_builder;
};

}   // namespace kiln::app

namespace kiln::app {

auto Builder::build() && -> App
{
    App result{};

    result.resources() = std::move(m_resource_context_builder).build();

    return result;
}

}   // namespace kiln::app
