module;

#include <GLFW/glfw3.h>

export module kiln.wsi.MonitorHandle;

namespace kiln::wsi {

export class MonitorHandle {
public:
    explicit MonitorHandle(GLFWmonitor* const monitor) : m_handle{ monitor } {}

    auto operator=(GLFWmonitor* const monitor) -> MonitorHandle&
    {
        m_handle = monitor;
        return *this;
    }

    [[nodiscard]]
    auto operator==(const MonitorHandle& other) const noexcept -> bool
    {
        return m_handle == other.m_handle;
    }

    [[nodiscard]]
    auto operator==(std::nullptr_t) const noexcept -> bool
    {
        return m_handle == nullptr;
    }

    [[nodiscard]]
    auto get() const noexcept -> GLFWmonitor*
    {
        return m_handle;
    }

private:
    GLFWmonitor* m_handle;
};

}   // namespace kiln::wsi
