#include <filesystem>
#include <source_location>
#include <span>

import kiln;

import examples.simple_scene.Context;

[[nodiscard]]
auto default_model_path() -> std::filesystem::path
{
    return std::filesystem::path{ std::source_location::current().file_name() }
               .parent_path()
               .parent_path()
               .parent_path()
         / "assets"
         / "models"
         / "Sponza"
         / "Sponza.gltf";
}

[[nodiscard]]
auto model_path_from(const std::span<const char* const> args) -> std::filesystem::path
{
    for (std::size_t index{ 1 }; index < args.size(); ++index)
    {
        if (args[index][0] != '-')
        {
            return std::filesystem::path{ args[index] };
        }
    }
    return default_model_path();
}

[[nodiscard]]
auto should_limit_fps(const std::span<const char* const> args) noexcept -> bool
{
    for (const char* const arg : args)
    {
        if (std::strcmp(arg, "--disable-fps-cap") == 0)
        {
            return false;
        }
    }
    return true;
}

auto main(const int argc, const char* const argv[]) -> int
{
    const std::span args{ argv, static_cast<std::size_t>(argc) };

    kiln::app::App app =                    //
        kiln::app::create("Simple scene")   //
            .use_context<kiln::gfx::vulkan::DebugMessenger>()
            .use_context<demo::Context>()
            .build();

    app.contexts()
        .at<demo::Context>()
        .run(app, model_path_from(args), should_limit_fps(args));
}
