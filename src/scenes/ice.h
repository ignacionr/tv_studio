#include "../sdladapters.h"
#include "../character.h"

class IceScene
{
    using CharacterType = Character<sdl::Renderer, sdl::EventPump::EventType>;

private:
    static constexpr int WindowWidth{700};
    static constexpr int WindowHeight{400};
    static constexpr int SceneWidth{7000};
    static constexpr int ParallaxPlanes{4};

    static constexpr int ForegroundLayerIndex{1};
    static constexpr int MidLayerIndex{2};

    static constexpr int IceInitialX{1000};
    static constexpr int IceInitialY{300};
    static constexpr int IceWidth{80};
    static constexpr int IceHeight{80};

    static constexpr int CatInitialX{0};
    static constexpr int CatInitialY{300};
    static constexpr int CatWidth{69};
    static constexpr int CatHeight{60};

    static constexpr int CatCols{10};
    static constexpr int CatRows{8};

    static constexpr unsigned int FrameDelayMs{16};
    static constexpr uint32_t DefaultCaptureFps{24};

public:
    static auto run(bool show_camera_pos = false, bool has_capture = false, uint32_t capture_start = 0, uint32_t capture_end = 0, uint32_t capture_fps = DefaultCaptureFps) noexcept -> void
    {
        try
        {
            sdl::EventPump pump;
            sdl::Window win("Runner?", WindowWidth, WindowHeight, pump);
            CharacterType::SceneType scene1(SceneWidth, WindowHeight, ParallaxPlanes);
            scene1.back()->background(std::string("rsrc/IMG_6110.jpg"));
            auto renderer{win.renderer()};
            Camera<CharacterType::RendererType, CharacterType::SceneType> cam1(renderer, show_camera_pos);
            if (has_capture)
            {
                cam1.configure_capture(capture_start, capture_end, capture_fps);
            }
            cam1.scene(&scene1);

            int x = IceInitialX;
            auto ice = std::make_shared<CharacterType>();
            auto *const p = ice.get();
            p->position_ = {x, IceInitialY, IceWidth, IceHeight};
            Sprite sprite_ice(*renderer, "rsrc/ice-block.png");
            sprite_ice.setupCharacter(*p);
            scene1.at(ForegroundLayerIndex)->push_back(ice);

            auto cat = std::make_shared<CharacterType>();
            cat->position_ = {CatInitialX, CatInitialY, CatWidth, CatHeight};
            Sprite sprite_cat(*renderer, "rsrc/cat-tile.png", CatCols, CatRows);
            sprite_cat.setupCharacter(*cat);
            // HMove(p->position_, 0, units::Speed::MetresPerSecond(9.0), *cat);
            cam1.follow(cat.get());
            scene1.at(MidLayerIndex)->push_back(cat);

            pump.run(
                [&]() -> void {
                    cam1.update();
                    scene1.update();
                    cam1.render();
                },
                FrameDelayMs,
                std::bind(&CharacterType::SceneType::handle_event, &scene1, std::placeholders::_1));
        }
        catch (sdl::Error &err)
        {
            std::cerr << err.what() << "\n";
        }
    }
};
