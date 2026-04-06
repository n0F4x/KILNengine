module;

#include <cassert>

#include <GLFW/glfw3.h>

export module kiln.wsi.Context;

import kiln.app.context.ContextBuilderInterface;
import kiln.util.type_traits.const_like;
import kiln.wsi.Error;

namespace kiln::wsi {

namespace internal {

export class ContextBuilder;

}   // namespace internal

export class Context {
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

export class ContextBuilder : public app::ContextBuilderInterface {
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

Context::Context()
{
    if (const bool success = glfwInit(); !success)
    {
        const char* error_description{};
        [[maybe_unused]]
        const int error_code = glfwGetError(&error_description);
        assert(
            (error_code == GLFW_PLATFORM_UNAVAILABLE || error_code == GLFW_PLATFORM_ERROR)
            && "Other error codes are unspecified"
        );

        throw Error{ error_description };
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

        const char* error_description{};
        [[maybe_unused]]
        const int error_code = glfwGetError(&error_description);
        assert(
            (error_code == GLFW_NO_ERROR || error_code == GLFW_PLATFORM_ERROR)
            && "Other error codes are unspecified"
        );
        assert(
            error_code != GLFW_PLATFORM_ERROR
            && "A bug or configuration error in GLFW,"
               " the underlying operating system or its drivers,"
               " or a lack of required resources "
               "- Issue a ticket to GLFW"
        );
    }
}

}   // namespace kiln::wsi
