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

using CharacterType = Character<sdl::Renderer, sdl::EventPump::EventType>; // character is implemented through sdl - need to have renderer and event

class ForestScene
{
private:
    static constexpr int WindowWidth{592};
    static constexpr int WindowHeight{272};
    static constexpr int SceneWidthMultiplier{10};
    static constexpr int SceneWidth{WindowWidth * SceneWidthMultiplier};
    static constexpr int ParallaxPlanes{5};

    static constexpr int BackgroundLayerIndex{4};
    static constexpr int FarTreesLayerIndex{3};
    static constexpr int MidTreesLayerIndex{2};
    static constexpr int ForegroundLayerIndex{1};
    static constexpr int CloseTreesLayerIndex{0};

    static constexpr int IceInitialX{500};
    static constexpr int IceInitialY{210 - 80};
    static constexpr int IceWidth{80};
    static constexpr int IceHeight{80};

    static constexpr int GirlInitialX{0};
    static constexpr int GirlInitialY{210 - 64};
    static constexpr int GirlWidth{32};
    static constexpr int GirlHeight{64};

    static constexpr int GirlCols{1};
    static constexpr int GirlRows{4};
    static constexpr int GirlAnimationSpeed{180};

    static constexpr double JumpDistance{10.0};
    static constexpr double JumpSpeed{4.0};
    static constexpr uint32_t JumpDurationMs{3000};

    static constexpr unsigned int FrameDelayMs{16};
    static constexpr uint32_t DefaultCaptureFps{24};

public:
    static auto run(bool show_camera_pos = false, bool has_capture = false, uint32_t capture_start = 0, uint32_t capture_end = 0, uint32_t capture_fps = DefaultCaptureFps) noexcept -> void
    {
        try
        {
            sdl::EventPump pump; // event pump is a loop. distribute events
            sdl::Window win("Demon Forest", WindowWidth, WindowHeight, pump);
            CharacterType::SceneType scene1(SceneWidth, WindowHeight, ParallaxPlanes);                       // create a scene type of character
            scene1.at(BackgroundLayerIndex)->background(std::string("rsrc/backgrounds/bg.png"));        // Adding backgrounds
            scene1.at(FarTreesLayerIndex)->background(std::string("rsrc/backgrounds/far_trees.png")); // set planese
            scene1.at(MidTreesLayerIndex)->background(std::string("rsrc/backgrounds/mid_trees.png")); // - layer - add 2m between layers (decided by cam)
            scene1.at(CloseTreesLayerIndex)->background(std::string("rsrc/backgrounds/close_trees.png"));
            auto renderer{win.renderer()};                                                // uniform initialization systax
            Camera<CharacterType::RendererType, CharacterType::SceneType> cam1(renderer, show_camera_pos); // create a camera of a render type and scene type- character type is like namespace
            if (has_capture)
            {
                cam1.configure_capture(capture_start, capture_end, capture_fps);
            }
            cam1.scene(&scene1);                                                          // add scene to the camera - cam.scene accepts pointer to the address of scene1

            // template instantiation: take the name of the template (such as "std::vector")
            // and a number of parameters inside angle brackets (such as "<int>")
            // std::vector<int> integerVector;
            // std::vector<std::string> stringVector;
            // std::map<int,std::string> mapOfIntsIntoStrings;

            int x = IceInitialX;
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
            ice->position_ = {x, IceInitialY, IceWidth, IceHeight};
            Sprite sprite_ice(*renderer, "rsrc/ice-block.png"); // passing pointer of the sharedpointer - *renderer (get object from the address)
            sprite_ice.addAnimation();
            sprite_ice.setupCharacter(*ice);                    // we have the address. we passing the object
            //Prosecution<CharacterType, CharacterType::SceneType, HMove<CharacterType>> prosecution1(*ice, nullptr, false);
            //HMove
            //HMove(HDirection.,)
            // HMove(HDirection::left, 0, units::Speed::MetresPerSecond(2), *ice);
            // change it into Pursue(xx,x,xx); (keep the reference)
            // Prosecution proc(xxx,xx,x,x,x,x);

            // we derreference (*) a pointer (an address) so that we get the object pointed to
            // int x{5};
            // int *p = &x; // now p is a pointer to the address of x
            // int y {*p}; // now y is initialized to the value of x
            // we use the * derreference operator, to get the object from the address
            // & operator address-of is the opposite of * operator derreference
            scene1.at(ForegroundLayerIndex)->push_back(ice); // lifetime of the character is not tied to the lifetime of the scene.

            auto girl = std::make_shared<CharacterType>();
            girl->position_ = {GirlInitialX, GirlInitialY, GirlWidth, GirlHeight};
            Sprite sprite_girl(*renderer, "rsrc/sprites/characters/spr_kanako_walk_.png", GirlCols, GirlRows); // what is 180? - speed of the animation
            sprite_girl.addAnimation("walkRight", 0, 3, GirlAnimationSpeed);
            sprite_girl.addAnimation("walkLeft", 0, 3, GirlAnimationSpeed, SDL_FLIP_HORIZONTAL);
            sprite_girl.chooseAnimation("walkRight");
            sprite_girl.setupCharacter(*girl);

            // HMove(ice->position_, 0, units::Speed::MetresPerSecond(10.0), *girl); // passing the function to get the target rectangle (at every update)
            Prosecution<CharacterType, CharacterType::SceneType, HMove<CharacterType>> prosecution(*girl, *ice, false,
            {
                {HDirection::left, [&sprite_girl]() -> void { sprite_girl.chooseAnimation("walkLeft"); }},
                {HDirection::right, [&sprite_girl]() -> void { sprite_girl.chooseAnimation("walkRight"); }}
            });

            Jump<CharacterType, CharacterType::SceneType, VMove<CharacterType>> jump1(*girl, units::Distance::Metres(JumpDistance), units::Speed::MetresPerSecond(JumpSpeed), JumpDurationMs);

            cam1.follow(girl.get());
            scene1.at(MidTreesLayerIndex)->push_back(girl);

            // run of the pump 1) Get message from the user 
            pump.run(
                [&]() -> void {
                    // update and render run by different cors
                    cam1.update();
                    scene1.update();
                    cam1.render();
                },
                FrameDelayMs, // do at most 60 frames per second (1000/ 60 ≃ 16)
                // "wait at least 16 ms between rendering"
                // creates a std::function that takes placeholder_1 (one parameter) into handle_event
                // and it implicitly uses scene1 as a "this" pointer for handle_event
                // std::bind(&CharacterType::SceneType::handle_event, &scene1, std::placeholders::_1) - i want to call handle_event in scene1
                // it is the same as:
                [&scene1](auto ev) -> auto { return scene1.handle_event(ev); }

            ); // bind handle event to scene ptr?
        }
        catch (sdl::Error &err)
        {
            std::cerr << err.what() << "\n";
        }
    }
};
