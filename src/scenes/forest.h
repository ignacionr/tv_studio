#include <functional>
#include <iostream>

#include "../sdladapters.h"
#include "../scene.h"
#include "../camera.h"
#include "../sprite.h"
#include "../move.h"
#include "../units.h"
#include "../character.h"
#include "../will/prosecution.h"
#include "../will/jump.h"

typedef Character<sdl::Renderer, sdl::EventPump::EventType> CharacterType; // character is implemented through sdl - need to have renderer and event

class ForestScene
{
public:
    static void run(bool show_camera_pos = false) noexcept
    {
        try
        {
            sdl::EventPump pump; // event pump is a loop. distribute events
            sdl::Window win("Demon Forest", 592, 272, pump);
            CharacterType::SceneType scene1(592 * 10, 272, 5);                       // create a scene type of character
            scene1.at(4)->background(std::string("rsrc/backgrounds/bg.png"));        // Adding backgrounds
            scene1.at(3)->background(std::string("rsrc/backgrounds/far_trees.png")); // set planese
            scene1.at(2)->background(std::string("rsrc/backgrounds/mid_trees.png")); // - layer - add 2m between layers (decided by cam)
            scene1.at(0)->background(std::string("rsrc/backgrounds/close_trees.png"));
            auto renderer{win.renderer()};                                                // uniform initialization systax
            Camera<CharacterType::RendererType, CharacterType::SceneType> cam1(renderer, show_camera_pos); // create a camera of a render type and scene type- character type is like namespace
            cam1.scene(&scene1);                                                          // add scene to the camera - cam.scene accepts pointer to the address of scene1

            // template instantiation: take the name of the template (such as "std::vector")
            // and a number of parameters inside angle brackets (such as "<int>")
            // std::vector<int> integerVector;
            // std::vector<std::string> stringVector;
            // std::map<int,std::string> mapOfIntsIntoStrings;

            int x = 500;
            auto ice = std::make_shared<CharacterType>(); // shared ptr of character type (raw pointer and reference count)
            // smart pointers (std::unique_ptr and std::shared_ptr) keep the concept of RAII
            // Resource Acquisition Is Initialization
            // when you instantiate a variable, a resource is created together with it
            // when your variable goes out of scope, the resource is claimed by the destructor
            // as if by automatically removing itself
            // std::make_shared<> create return ptr and return it
            // {
            //     std::ofstream ofs("hello.txt"); // we acquire a file
            //     ofs << "Hello World!" << std::endl;
            //     // automatically close the file
            // }

            // ice->position_; // the arrow operator of a smart pointer, will help by resolving the ptr into raw ptr
            // ice.get()->position_; // this is the equivalent of the previous

            // auto p = ice.get(); // return the raw pointer to p from shared ptr ice
            ice->position_ = {x, 210 - 80, 80, 80};
            Sprite sprite_ice(*renderer, "rsrc/ice-block.png"); // passing pointer of the sharedpointer - *renderer (get object from the address)
            sprite_ice.addAnimation();
            sprite_ice.setupCharacter(*ice);                    // we have the address. we passing the object
            //Prosecution<CharacterType, CharacterType::SceneType, HMove<CharacterType>> prosecution1(*ice, nullptr, false);
            //HMove
            //HMove(HDirection.,)
            HMove(HDirection::left, 0, units::Speed::MetresPerSecond(2), *ice);
            // change it into Pursue(xx,x,xx); (keep the reference)
            // Prosecution proc(xxx,xx,x,x,x,x);

            // we derreference (*) a pointer (an address) so that we get the object pointed to
            // int x{5};
            // int *p = &x; // now p is a pointer to the address of x
            // int y {*p}; // now y is initialized to the value of x
            // we use the * derreference operator, to get the object from the address
            // & operator address-of is the opposite of * operator derreference
            scene1.at(1)->push_back(ice); // why not *ice? - lifetime of the character is not tied to the lifetime of the scene. since we don't have garbage collection in cpp we use shared ptr

            auto girl = std::make_shared<CharacterType>();
            girl->position_ = {0, 210 - 64, 32, 64};
            Sprite sprite_girl(*renderer, "rsrc/sprites/characters/spr_kanako_walk_.png", 1, 4); // what is 180? - speed of the animation
            sprite_girl.addAnimation("walkRight",0, 3, 180);
            sprite_girl.addAnimation("walkLeft",0, 3, 180, SDL_FLIP_HORIZONTAL);
            sprite_girl.chooseAnimation("walkRight");
            sprite_girl.setupCharacter(*girl);

            // HMove(ice->position_, 0, units::Speed::MetresPerSecond(10.0), *girl); // passing the function to get the target rectangle (at every update)
            Prosecution<CharacterType, CharacterType::SceneType, HMove<CharacterType>> prosecution(*girl, *ice, false,
            {
                {HDirection::left, [&sprite_girl](){ sprite_girl.chooseAnimation("walkLeft");}},
                {HDirection::right, [&sprite_girl](){ sprite_girl.chooseAnimation("walkRight");}}
            });

            Jump<CharacterType, CharacterType::SceneType, VMove<CharacterType>> jump1(*girl, units::Distance::Metres(10.0), units::Speed::MetresPerSecond(4.0), 3000);

            cam1.follow(girl.get());
            scene1.at(2)->push_back(girl);

            // run of the pump 1) Get message from the user 
            pump.run(
                [&]() {
                    // update and render run by different cors
                    cam1.update();
                    scene1.update();
                    cam1.render();
                },
                16, // do at most 60 frames per second (1000/ 60 ≃ 16)
                // "wait at least 16 ms between rendering"
                // creates a std::function that takes placeholder_1 (one parameter) into handle_event
                // and it implicitly uses scene1 as a "this" pointer for handle_event
                // std::bind(&CharacterType::SceneType::handle_event, &scene1, std::placeholders::_1) - i want to call handle_event in scene1
                // it is the same as:
                [&scene1](auto ev) { return scene1.handle_event(ev); }

            ); // bind handle event to scene ptr?
        }
        catch (sdl::Error &err)
        {
            std::cerr << err.what() << std::endl;
        }
    }
};
