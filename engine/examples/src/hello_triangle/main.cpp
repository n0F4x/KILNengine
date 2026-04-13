#include <atomic>
#include <chrono>
#include <print>
#include <thread>

import kiln;
import hello_triangle;

auto run(kiln::app::App& app) -> void;

auto main() -> int
{
    kiln::app::App app =   //
        kiln::app::create("Hello triangle!")
            .use_context<kiln::gfx::vulkan::DebugMessenger>()
            .use_context<Demo>()
            .build();

    std::println(
        "Created renderer using {}",
        app.contexts().at<kiln::gfx::renderer::Device>().name()
    );

    run(app);
}

auto run(kiln::app::App& app) -> void
{
    using namespace std::chrono_literals;

    const kiln::wsi::Context& wsi_context{ app.contexts().at<kiln::wsi::Context>() };
    Demo&                     demo{ app.contexts().at<Demo>() };
    std::atomic_bool          running{ true };
    std::atomic_bool          window_resized{ false };
    std::atomic               window_resolution{ demo.window().resolution() };

    std::jthread render_thread{
        [&demo, &running, &window_resized, &window_resolution] mutable -> void
        {
            auto last_time{ std::chrono::steady_clock::now() };
            while (running)
            {
                const auto now{ std::chrono::steady_clock::now() };
                const auto delta_time{ now - last_time };


                if (window_resized.exchange(false))
                {
                    demo.on_window_resize(window_resolution);
                }

                demo.render();


                std::this_thread::sleep_for(1s / 60 - delta_time);
                last_time = now;
            }
        }   //
    };

    while (!demo.window().should_close())
    {
        kiln::wsi::wait_events(wsi_context);

        if (demo.window().key_pressed(kiln::wsi::Key::eEscape))
        {
            demo.window().request_close();
        }

        // TODO: only do this on a window resize event
        window_resolution = demo.window().resolution();
        window_resized    = true;
    }

    running = false;
    render_thread.join();

    demo.shut_down();
}
