module;

#include <utility>

#include <tsl/ordered_map.h>

export module kiln.app.ContextBuilder;

import kiln.app.Context;

namespace kiln::app {

export class ContextBuilder {
public:
    [[nodiscard]]
    auto build() && -> Context;

private:
    Context m_context;
};

}   // namespace kiln::app

namespace kiln::app {

auto ContextBuilder::build() && -> Context
{
    return std::move(m_context);
}

}   // namespace kiln::app
