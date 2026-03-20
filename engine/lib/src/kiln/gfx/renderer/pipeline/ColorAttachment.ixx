module;

#include <array>
#include <optional>
#include <span>
#include <utility>

#include "kiln/util/lifetimebound.hpp"

export module kiln.gfx.renderer.pipeline.ColorAttachment;

namespace kiln::gfx::renderer {

export class ColorAttachment {
public:
    [[nodiscard]]
    auto clear_value() const noexcept [[kiln_lifetimebound]]
    -> std::optional<std::span<const float, 4>>;

    template <typename Self_T>
    auto set_clear_value(this Self_T&&, std::span<const float, 4> value) noexcept
        -> Self_T&&;

private:
    std::optional<std::array<float, 4>> m_clear_value;
};

}   // namespace kiln::gfx::renderer

namespace kiln::gfx::renderer {

inline auto ColorAttachment::clear_value() const noexcept
    -> std::optional<std::span<const float, 4>>
{
    return m_clear_value.transform(
        [](const std::array<float, 4>& value) -> std::span<const float, 4>
        {
            return value;   //
        }
    );
}

template <typename Self_T>
auto ColorAttachment::set_clear_value(
    this Self_T&&                   self,
    const std::span<const float, 4> value
) noexcept -> Self_T&&
{
    self.ColorAttachment::m_clear_value = std::array{
        value[0],
        value[1],
        value[2],
        value[3],
    };
    return std::forward<Self_T>(self);
}

}   // namespace kiln::gfx::renderer
