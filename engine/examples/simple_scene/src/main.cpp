#include <filesystem>
#include <iostream>
#include <source_location>

import kiln;

auto main(const int argc, const char* const argv[]) -> int
{
    if (argc < 2)
    {
        std::cerr << "provide a gltf asset as program argument\n";
        return -1;
    }

    kiln::app::App app =   //
        kiln::app::create("Simple scene")
            .use_context<kiln::res::ResourceManager>()
            .apply_bundle(kiln::gfx::Bundle{})
            .build();

    return !app.contexts()
                .at<kiln::gfx::asset::gltf::Loader>()
                .load(
                    std::filesystem::path{ std::source_location::current().file_name() }
                            .parent_path()
                            .parent_path()
                        / "res" / argv[1],
                    app.contexts().at<kiln::res::ResourceManager>()
                )
                .has_value();
}
