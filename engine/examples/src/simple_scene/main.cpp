#include <filesystem>
#include <source_location>

import kiln;

import examples.simple_scene.Context;

[[nodiscard]]
auto assets_path() -> std::filesystem::path
{
    return std::filesystem::path{ std::source_location::current().file_name() }
               .parent_path()
               .parent_path()
               .parent_path()
         / "assets";
}

[[nodiscard]]
auto default_model_path() -> std::filesystem::path
{
    return assets_path() / "models" / "Sponza" / "Sponza.gltf";
}

auto main(const int argc, const char* const argv[]) -> int
{
    kiln::app::App app =                    //
        kiln::app::create("Simple scene")   //
            .use_context<kiln::gfx::vulkan::DebugMessenger>()
            .use_context<demo::Context>()
            .build();

    const std::filesystem::path model_path{ argc < 2 ? default_model_path() : argv[1] };

    app.contexts().at<demo::Context>().run(model_path);
}
