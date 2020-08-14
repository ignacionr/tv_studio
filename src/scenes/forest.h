#include <functional>
#include <iostream>

#include "../sdladapters.h"
#include "../scene.h"
#include "../camera.h"
#include "../sprite.h"
#include "../move.h"
#include "../units.h"
#include "../character.h"

typedef Character<sdl::Renderer, sdl::EventPump::EventType> CharacterType;

class ForestScene
{
public:
    static void run() noexcept
    {
        try
        {
            sdl::EventPump pump;
            sdl::Window win("Demon Forest", 700, 272 * 2, pump);
            CharacterType::SceneType scene1(960, 272 * 2, 5);
            scene1.at(4)->background(std::string("rsrc/sprites/backgrounds/bg.png"));
            scene1.at(3)->background(std::string("rsrc/sprites/backgrounds/far_trees.png"));
            scene1.at(2)->background(std::string("rsrc/sprites/backgrounds/mid_trees.png"));
            scene1.at(0)->background(std::string("rsrc/sprites/backgrounds/close_trees.png"));
            auto renderer{win.renderer()}; // uniform initialization systax
            Camera<CharacterType::RendererType, CharacterType::SceneType> cam1(renderer);
            cam1.scene(&scene1);

            int x = 1000;
            auto ice = std::make_shared<CharacterType>();
            auto p = ice.get();
            p->_position = {x, 300, 80, 80};
            Sprite sprite_ice(*renderer, "rsrc/ice-block.png");
            sprite_ice.setupCharacter(*p);
            scene1.at(1)->push_back(ice);

            auto girl = std::make_shared<CharacterType>();
            girl->_position = {0, 300, 32, 64};
            Sprite sprite_girl(*renderer, "rsrc/sprites/characters/spr_kanako_walk_.png", 1, 4, 0, 3, 180);
            sprite_girl.setupCharacter(*girl);
            HMove(p->_position, 0, units::Speed::MetresPerSecond(8.0), *girl);
            scene1.at(2)->push_back(girl);

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