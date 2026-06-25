#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <map>

// TODO: create a mechanism by which we can specify different move animations possibly with names, in order to be used by disconnected components
// for example: "jumping" "running" -they would be invoked by actions or updates of any kind

template <typename TRenderer>
struct Sprite
{
    Sprite(TRenderer const &renderer, std::string const &imagefile, int rows = 1, int cols = 1) // sprite sheet?? want to undestand
        : rows_{rows}, cols_{cols}
    {
        texture_ = renderer.CreateTexture(imagefile, &dimensions_.w, &dimensions_.h);
        dimensions_.h /= rows;
        dimensions_.w /= cols;
    }

    void addAnimation(std::string const &name = "default", int from =0, int to=-1, int step_division=-1, SDL_RendererFlip flip = SDL_FLIP_NONE) 
    {
        if (to == -1)
            to = from;
        if (step_division == -1)
            step_division = 200 / (to - from + 1);
        animations_[name] = Animation{from, to, step_division, flip};
        chooseAnimation(name);
    }

    void chooseAnimation(std::string const &name) 
    {
        current_animation_ = &animations_.at(name);
    }

    template <typename TCharacter>
    void setupCharacter(TCharacter &character)
    {
        character.render_ = [&](TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> translator) { // lamda expression?
            if (current_animation_) {
                auto destination = translator(character.position_);
                auto cell = (current_step_ % (current_animation_->loop_to_ - current_animation_->loop_from_ + 1)) + current_animation_->loop_from_;
                typename TCharacter::RendererType::RectType
                    from{dimensions_.w * (cell % cols_), dimensions_.h * (cell / cols_),
                        dimensions_.w, dimensions_.h};
                renderer->CopyEx(*texture_, &from, &destination, 0, nullptr, current_animation_->flip_);
            }
        };
        character.addUpdate([&](typename TCharacter::SceneType *scene) {
            if (current_animation_)
                current_step_ = scene->age() / current_animation_->step_divisor_;
            return true;
        });
    }

private:
    std::shared_ptr<typename TRenderer::TextureType> texture_;
    typename TRenderer::RectType dimensions_;
    const int rows_;
    const int cols_;
    struct Animation {
        int loop_from_, loop_to_, step_divisor_;
        SDL_RendererFlip flip_;
    };
    std::map<std::string,Animation> animations_;
    int current_step_{0};
    const Animation *current_animation_ {nullptr};
};
