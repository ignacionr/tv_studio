#include "../sdladapters.h"
#include "../character.h"

class IceScene
{
    using CharacterType = Character<sdl::Renderer, sdl::EventPump::EventType>;

public:
    static void run() noexcept
    {
        try
        {
            sdl::EventPump pump;
            sdl::Window win("Runner?", 700, 400, pump);
            CharacterType::SceneType scene1(7000, 400, 4);
            scene1.back()->background(std::string("rsrc/IMG_6110.jpg"));
            auto renderer{win.renderer()};
            Camera<CharacterType::RendererType, CharacterType::SceneType> cam1(renderer);
            cam1.scene(&scene1);

            int x = 1000;
            auto ice = std::make_shared<CharacterType>();
            auto p = ice.get();
            p->position_ = {x, 300, 80, 80};
            Sprite sprite_ice(*renderer, "rsrc/ice-block.png");
            sprite_ice.setupCharacter(*p);
            scene1.at(1)->push_back(ice);

            auto cat = std::make_shared<CharacterType>();
            cat->position_ = {0, 300, 69, 60};
            Sprite sprite_cat(*renderer, "rsrc/cat-tile.png", 10, 8);
            //  46, 53, 95
            sprite_cat.setupCharacter(*cat);
            // HMove(p->position_, 0, units::Speed::MetresPerSecond(9.0), *cat);
            scene1.at(2)->push_back(cat);

            pump.run(
                [&]() {
                    cam1.update();
                    scene1.update();
                    cam1.render();
                },
                16,
                std::bind(&CharacterType::SceneType::handle_event, &scene1, std::placeholders::_1));
        }
        catch (sdl::Error &err)
        {
            std::cerr << err.what() << std::endl;
        }
    }
};
