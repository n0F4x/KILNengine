module;

#include <filesystem>

export module simple_scene;

import kiln.app.App;
import kiln.app.Builder;

namespace demo {

export struct Bundle {
    static auto operator()(kiln::app::Builder& builder) -> void;
};

export auto run(kiln::app::App& app, const std::filesystem::path& model_filepath) -> void;

}   // namespace demo
