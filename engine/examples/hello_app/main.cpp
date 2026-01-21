#include <print>
#include <string>

#include <kiln/app.hpp>

auto main() -> int
{
    using namespace kiln;

    app::App app = app::create()   //
                       .inject_resource([] -> int { return 42; })
                       .inject_resource([](const int& other_resource) -> std::string {
                           return std::to_string(other_resource);
                       })
                       .build();

    std::println("{}", app.resources().at<std::string>());
}
