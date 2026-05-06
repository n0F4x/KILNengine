module;

#include <GLFW/glfw3.h>

export module kiln.wsi.WindowHandle;

namespace kiln::wsi {

export class WindowHandle {
public:
    explicit WindowHandle(GLFWwindow* const window) : m_handle{ window } {}

    auto operator=(GLFWwindow* const window) -> WindowHandle&
    {
        m_handle = window;
        return *this;
    }

    [[nodiscard]]
    auto operator==(const WindowHandle& other) const noexcept -> bool
    {
        return m_handle == other.m_handle;
    }

    [[nodiscard]]
    auto operator==(std::nullptr_t) const noexcept -> bool
    {
        return m_handle == nullptr;
    }

    [[nodiscard]]
    auto get() const noexcept -> GLFWwindow*
    {
        return m_handle;
    }

private:
    GLFWwindow* m_handle;
};

}   // namespace kiln::wsi
