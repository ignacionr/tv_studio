#pragma once
#include <functional>
#include <memory>
#include <utility>

template <typename TRenderable>
struct Scene;

// TODO: change the signature of updates, so that they not only take a Scene, but also the age at which the last update was executed
// this will help moves figure out the new position, by starting with the current position
// that in turn, will allow for composition of moves (right now, moves are independent of the current position -they take from the start)

template <typename TScene>
std::function<bool(TScene *)> operator+(std::function<bool(TScene *)> const &a, std::function<bool(TScene *)> b) // a and b are two updates
{
    if (!a) // if pass function has nothing a return b
        return b;
    if (!b) // likewise above
        return a;
    auto pa = std::make_shared<std::function<bool(TScene *)>>(a); // pa is shared pointer to a
    auto pb = std::make_shared<std::function<bool(TScene *)>>(b);
    return [pa, pb](TScene *scene) mutable { // lambda funct
        bool result{false};                  // result assign to false - bool result = false? (why not?)
        if (pa)
        {
            result = (*pa)(scene); // call pa and pass the scene
            if (!result)
                pa.reset(); // Destroys the object currently managed by the unique_ptr (if any) and takes ownership of p.
        }
        if (pb)
        {
            if ((*pb)(scene))
            {
                result = true;
            }
            else
            {
                pb.reset();
            }
        }
        return result;
    };
}

template <typename TScene> // line 62 - combine two update
std::function<bool(TScene *)> &operator+=(std::function<bool(TScene *)> &a, std::function<bool(TScene *)> b)
{
    a = a + std::move(b);
    return a;
}

// A Character is a renderable presence on a plane
// It may respond to events
template <typename TRenderer, typename TEvent>
struct Character
{
    using EventType = TEvent; //creates an alias that can be used anywhere in place of a (possibly complex) type name.
    using RendererType = TRenderer;
    // using is an alias for a type
    // int x; // defines a variable x of type int
    // using X = int; // defines a type X that is equivalent to int
    using SceneType = Scene<Character<TRenderer, TEvent>>; // scene is a vector of characters
    using UpdateType = std::function<bool(SceneType *)>;   // update type is a function accepting scene and return bool

    std::function<void(TRenderer *, std::function<typename TRenderer::RectType(typename TRenderer::RectType)>)> render_;
    std::function<bool(EventType *)> react_;
    typename TRenderer::RectType position_;

    void addUpdate(UpdateType update)
    {
        update_ += std::move(update); // combine two update
    }

    bool Update(SceneType *scene)
    {
        if (update_)
            return update_(scene); // update the scene and return true?
        return false;
    }

    auto dispersionCount() const
    {
        return 500;
    }

    auto moveFactory()
    {
        // we need to return something that will be called as:
        // returnedObject(SDL_Rect)
        // this needs to produce a shared_ptr<HMove> such that
        // HMove can be called to cancel (move.cancel())
        return [&](auto direction, uint32_t start_time) {
            // HOMEWORK: complete the list of parameters to the constructor of HMove
            return std::make_shared<HMove<Character>>(direction, start_time, units::Speed::MetresPerSecond(8.0), *this);
        };
    }

private:
    UpdateType update_;
};
