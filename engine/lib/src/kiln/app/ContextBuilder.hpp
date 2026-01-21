#pragma once

#include <utility>

#include "kiln/app/Context.hpp"

namespace kiln::app {

class ContextBuilder {
public:
    [[nodiscard]]
    auto build() && -> Context;

private:
    Context m_context;
};

}   // namespace kiln::app

namespace kiln::app {

inline auto ContextBuilder::build() && -> Context
{
    return std::move(m_context);
}

}   // namespace kiln::app
