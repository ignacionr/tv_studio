#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include "plane.h"

// A Scene is a collection of superposing Planes of the same size
template <typename TRenderable>
struct Scene
{
    using CharacterType = TRenderable;

    Scene(unsigned int w, unsigned int h, unsigned int z)
        : w_(w), h_(h)
    {
        planes_.reserve(z);
        for (; z; --z)
        {
            planes_.push_back(std::make_shared<Plane<TRenderable>>(w, h)); // assign plane to the render in reverse order
        }
        start_ = TRenderable::RendererType::GetTicks(); // get delta time?
    }

    auto pixel_size() const // get pixel size
    {
        return typename TRenderable::RendererType::RectType{0, 0, w_, h_};
    }

    auto age() const
    {
        return TRenderable::RendererType::GetTicks() - start_;
    }

    template <typename T>            // templeate for for each , it can be do any action
    void foreach_character(T action) // handle event for every character?
    {
        for (auto &plane : planes_)
        {
            for (auto &ch : *plane) // for each
            {
                action(ch);
            }
        }
    }

    bool handle_event(typename TRenderable::EventType *ev)
    {
        foreach_character([ev](auto &ch) { // for each react
            if (ch->react_)                // react_ ??
            {
                ch->react_(ev);
            }
        });
        return true;
    }

    void update()
    {
        foreach_character([this](auto &ch) { // for each update
            ch->Update(this);

            // Educational comment: We use std::clamp from <algorithm> to enforce boundary constraints.
            // This ensures that the character's position_.x never drops below 0 (left edge)
            // or exceeds w_ - ch->position_.w (right edge), preventing them from drifting
            // out of the scene viewport bounds.
            ch->position_.x = std::clamp(ch->position_.x, 0, w_ - ch->position_.w);
        });
    }

    auto back() const
    {
        return planes_.back();
    }

    auto at(int idx) const
    {
        return planes_.at(idx);
    }

    auto size() const { return planes_.size(); }

private:
    Uint32 start_;
    int w_, h_;
    std::vector<std::shared_ptr<Plane<TRenderable>>> planes_;
};
