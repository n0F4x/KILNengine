module;

#include <expected>
#include <memory_resource>
#include <vector>

#include <GLFW/glfw3.h>

export module kiln.wsi.WindowProxy;

import vulkan_hpp;

import kiln.util.StringLiteral;
import kiln.wsi.Context;
import kiln.wsi.event.Key;
import kiln.wsi.Size;
import kiln.wsi.WindowCommand;
import kiln.wsi.WindowSettings;

namespace kiln::wsi {

export class WindowProxy {
public:
    struct CreateInfo {
        util::StringLiteral title{ "" };
        WindowSettings      settings;
    };

    using allocator_type = std::pmr::polymorphic_allocator<>;


    WindowProxy(const WindowProxy&) = delete;
    WindowProxy(WindowProxy&&)      = default;
    WindowProxy(WindowProxy&&, const allocator_type&);

    explicit WindowProxy(const Context& context, const CreateInfo& create_info);
    explicit WindowProxy(
        std::allocator_arg_t,
        const allocator_type& allocator,
        const Context&        context,
        const CreateInfo&     create_info
    );

    auto operator=(WindowProxy&&) -> WindowProxy& = default;


    [[nodiscard]]
    auto get_allocator() const noexcept -> allocator_type;

    [[nodiscard]]
    auto context() const noexcept -> const Context&;


    auto destroy() -> void;

    [[nodiscard]]
    auto create_vulkan_surface(const vk::raii::Instance& instance)
        -> std::expected<vk::raii::SurfaceKHR, vk::Result>;


    [[nodiscard]]
    auto flush_changes(const std::pmr::vector<WindowCommand>::allocator_type& allocator)
        -> std::pmr::vector<WindowCommand>;

private:
    Context                         m_context;
    GLFWwindow*                     m_handle;
    bool                            m_destroyed{};
    std::pmr::vector<WindowCommand> m_changes;
};

}   // namespace kiln::wsi
