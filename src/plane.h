#pragma once
#include <functional>
#include <memory>
#include <iostream>
#include <utility>

// A Plane is one positioning line along the x,y axis
template <typename TRenderable>
struct Plane : public std::list<std::shared_ptr<TRenderable>> // inherit from list of TRenderable shared ptr
{
    using TRenderer = typename TRenderable::RendererType;

    class ImageBackground // nested class (within class plane)
    {
    public:
        ImageBackground(int const &w, int const &h, std::string filename) // set image background // NOLINT(bugprone-easily-swappable-parameters)
            : w_{w}, h_{h}, filename_{std::move(filename)} {}

        auto operator()(TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> const &translator) // operator function
        {
            if (!background_texture_)
            {
                background_texture_ = renderer->CreateTexture(filename_, &imageW_, &imageH_);
            }
            int actual_width = 0;
            for (int x {0}; x < w_; x += actual_width)
            {
                typename TRenderer::RectType rc{x, 0, imageW_, imageH_}; // start from 0,0
                // the translator will turn our original rectangle, into a new rectangle with gaps on the sides
                // take into account the gap, and correct for it
                rc = translator(rc); // translation apply to rectangle
                actual_width = rc.w;
                renderer->Copy(*background_texture_, nullptr, &rc); // can't navigate?? :(
            }
        }

    private:
        // w_ is the width of the containing plane
        int const &w_; //plane will need two reference.  - backtracing can be done within declaration
        // h_ is the height of the containing plane
        int const &h_;
        // size of the image used as background
        int imageW_, imageH_;
        std::string filename_;
        std::shared_ptr<typename TRenderer::TextureType> background_texture_;
    };

    Plane(int w, int h) : w_(w), h_(h) {} // constructor? don't we have a destructor? // NOLINT(bugprone-easily-swappable-parameters)

    auto background(std::string const &filename) -> void
    {
        image_background_ = std::make_shared<ImageBackground>(w_, h_, std::move(filename)); // set background
        background_ = [this](auto renderer, auto const &translator) -> void {
                    auto &bg {*image_background_}; // bg initialized from reference of image_background_
                    bg(renderer, translator);
        };
        // std::bind(&ImageBackground::operator(), image_background_, std::placeholders::_1, std::placeholders::_2); // bind operator to
    }                   // image background ??? why??

    template <typename TColor>
    void background(TColor const &color) // background by color
    {
        background_ = [color, this](TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> const &translator) -> auto {
            typename TRenderer::RectType rc{0, 0, w_, h_};
            rc = translator(rc);
            renderer->SetDrawColor(color);
            renderer->FillRect(&rc);
        };
    }

    // camera will call this
    void render(TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> const &translator) const
    {
        // render background
        if (background_) {
            background_(renderer, translator);
}

        // render all characters
        for (auto &ch : *this) // foreach in c#
        {
            if (ch->render_) // does this character has a render
            {
                ch->render_(renderer, translator);
            }
        }
    }

private:
    int w_, h_;
    std::shared_ptr<ImageBackground> image_background_;
    std::function<void(TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> const &translator)> background_;
};
