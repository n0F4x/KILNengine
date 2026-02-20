module;

#include <optional>

export module kiln.wsi.WindowedWindowSettings;

import kiln.wsi.Size;

namespace kiln::wsi {

export struct WindowedWindowSettings {
    struct Border {
        bool resizable{ true };
    };

    enum struct Visibility
    {
        eDefault,
        eHidden,
        eFocused,
    };

    Size2i                content_size;
    std::optional<int>    position_x;
    std::optional<int>    position_y;
    std::optional<Border> border{ std::in_place };
    Visibility            visibility{ Visibility::eDefault };
    bool                  maximized{ false };
    bool                  focus_on_show{ true };
    bool                  scale_to_monitor{ false };
};

}   // namespace kiln::wsi
