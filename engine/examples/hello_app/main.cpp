#include <print>
#include <string>
#include <utility>

#include <kiln/app.hpp>

auto custom_plugin(kiln::app::App& app) -> void
{
    std::println("Injected resource is {}", app.resources().at<int>());
}

auto main() -> int
{
    using namespace kiln;

    std::ignore = app::create()
                      .inject_resource([] -> int { return 42; })
                      .plug_in([](app::ResourcePlugin& resource_plugin) -> auto {
                          resource_plugin.inject_resource(
                              [](const int& other_resource) -> std::string {
                                  return std::to_string(other_resource);
                              }
                          );
                          return custom_plugin;
                      })
                      .build();
}
