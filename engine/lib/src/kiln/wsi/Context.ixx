module;

#include <GLFW/glfw3.h>

export module kiln.wsi.Context;

import kiln.app.registry.EntryBase;
import kiln.app.registry.EntryBuilderInterface;
import kiln.util.type_traits.const_like;
import kiln.wsi.error.handle_glfw_error;

namespace kiln::wsi {

namespace internal {

export class ContextBuilder;

}   // namespace internal

export class Context : public app::EntryBase  {
public:
    using Builder = internal::ContextBuilder;


    Context();
    Context(const Context&);
    Context(Context&&) noexcept;
    ~Context();

    auto operator=(const Context&) -> Context&     = default;
    auto operator=(Context&&) noexcept -> Context& = default;

private:
    inline static uint32_t active_context_count{};
};

namespace internal {

export class ContextBuilder : public app::EntryBuilderInterface {
public:
    template <typename Self_T>
    [[nodiscard]]
    auto context(this Self_T& self) -> util::const_like_t<Context, Self_T>
    {
        return self.ContextBuilder::m_context;
    }

    [[nodiscard]]
    auto build() const -> Context
    {
        return m_context;
    }

private:
    Context m_context;
};

}   // namespace internal

}   // namespace kiln::wsi

module :private;

namespace kiln::wsi {

auto initialize() -> void
{
    glfwSetErrorCallback(handle_glfw_error);
    glfwInit();
}

Context::Context()
{
    if (active_context_count == 0)
    {
        initialize();
    }

    ++active_context_count;
}

Context::Context(const Context&)
{
    ++active_context_count;
}

Context::Context(Context&&) noexcept
{
    ++active_context_count;
}

Context::~Context()
{
    --active_context_count;

    if (active_context_count == 0)
    {
        glfwTerminate();
    }
}

}   // namespace kiln::wsi
