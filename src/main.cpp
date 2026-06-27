#include "sdladapters.h"
#include "scenes/forest.h"
#include "scenes/ice.h"

auto main(int argc, char* argv[]) -> int
{
    sdl::UsesSDL use_sdl;
    sdl::UsesFont use_font;

    bool show_camera_pos = false;
    bool has_capture = false;
    uint32_t capture_start = 0;
    uint32_t capture_end = 0;
    static constexpr uint32_t DefaultCaptureFps{24};
    uint32_t capture_fps = DefaultCaptureFps;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);
        if (arg == "-capture" && i + 1 < argc)
        {
            std::string spec(argv[i + 1]);
            size_t dash = spec.find('-');
            size_t at = spec.find('@');
            if (dash != std::string::npos && at != std::string::npos && at > dash)
            {
                try
                {
                    capture_start = std::stoul(spec.substr(0, dash));
                    capture_end = std::stoul(spec.substr(dash + 1, at - dash - 1));
                    capture_fps = std::stoul(spec.substr(at + 1));
                    has_capture = true;
                }
                catch (...)
                {
                    // Fail gracefully on parse errors
                }
            }
            ++i; // Skip the next argument as we consumed it
        }
        else
        {
            // Any other argument enables the position overlay
            show_camera_pos = true;
        }
    }

    ForestScene::run(show_camera_pos, has_capture, capture_start, capture_end, capture_fps);
    // IceScene::run(show_camera_pos, has_capture, capture_start, capture_end, capture_fps);
    return 0;
}