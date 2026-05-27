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
auto model_path_from(
    const std::span<const std::string_view> args,
    const std::filesystem::path&            default_value = default_model_path()
) -> std::filesystem::path
{
    for (std::size_t index{ 1 }; index < args.size(); ++index)
    {
        if (args[index][0] != '-')
        {
            return std::filesystem::path{ args[index] };
        }
    }
    return default_value;
}

[[nodiscard]]
auto should_limit_fps(const std::span<const std::string_view> args) noexcept -> bool
{
    return !std::ranges::contains(args, "--disable-fps-cap");
}

[[nodiscard]]
auto grid_size_from(
    const std::span<const std::string_view> args,
    const uint32_t                          default_value = 7
) -> uint32_t
{
    for (const std::string_view arg : args)
    {
        if (constexpr std::string_view option_initializer{ "--grid-size=" };
            arg.starts_with(option_initializer))
        {
            const std::string_view grid_size_string{
                arg.substr(option_initializer.size())
            };
            if (uint32_t result{};   //
                std::from_chars(
                    grid_size_string.data(),
                    grid_size_string.data() + grid_size_string.size(),
                    result
                )
                    .ec
                != std::errc{})
            {
                throw std::runtime_error{ "Invalid grid size given as argument" };
            }
            else
            {
                if (result == 0)
                {
                    throw std::runtime_error{
                        "Invalid grid size given as argument. Grid size cannot be 0"
                    };
                }
                return result;
            }
        }
    }

    return default_value;
}

auto main(const int argc, const char* const argv[]) -> int
{
    const std::vector<std::string_view> args{ argv, argv + argc };

    kiln::app::App app =                    //
        kiln::app::create("Simple scene")   //
            .use_context<kiln::gfx::vulkan::DebugMessenger>()
            .use_context<demo::Context>()
            .build();

    app.contexts()
        .at<demo::Context>()
        .run(app, model_path_from(args), should_limit_fps(args), grid_size_from(args));
}
