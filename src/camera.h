#pragma once
#include <memory>
#include <algorithm>
#include "units.h"

// A Camera produces renders by superposing the planes of a scene, from a perspective
// camera is a class has two parameters of render and scene
template <typename TRenderer, typename TScene>
struct Camera
{
    Camera(std::shared_ptr<TRenderer> renderer) // constructor by passing render shared_ptr
        : renderer_(renderer) { ; }

    void scene(TScene const *scene_param) // setting up scene to the camera
    {
        scene_ = scene_param;
        plane_translations_.resize(0); // why it resize to zero? initial vector size is not zero?
        camera_y_ = scene_param->pixel_size().h / 2; 
    }

    void render()
    {
        renderer_->Clear(); // clear the render
        if (plane_translations_.empty())
        {
            create_plane_translations();
        }
        const int planes = scene_->size(); // get number of planes
        for (int idx{planes - 1}; idx >= 0; --idx) // loop from highest value
        {
            scene_->at(idx)->render(renderer_.get(), plane_translations_[idx]);
        }
        renderer_->Present(); // SDL requirement
    }

    void update()
    {
        // camera_z_ = std::min({(-50) + (static_cast<int>(scene_->age()) / 100), -10});
        camera_x_ = scene_->age() / 20; // speed of the cam
        // zoom_ = scene_->age() * 0.00001 + 1.0;
        create_plane_translations();
    }

// C++ style guide allows private member variables at the end
private:
    void create_plane_translations()
    {
        const int planes = scene_->size();
        plane_translations_.resize(planes); // does this change at runtime?
        // int reduction{-camera_z_};
        int w, h;
        renderer_->GetDimensions(&w, &h); // out parameters ? why pass the address?
        // we pass the address-of each integer, so that the fn can modify the values
        // C# ref => initialized values, pass by-ref (by-ref in C++)
        // C# out => non-initialized values, pass by-ref (no equivalent)
        for (int idx = 0; idx < planes; ++idx)
        {
            auto reduction = static_cast<uint32_t>(log(
                (camera_z_ + distance_between_planes_ * idx) / aperture_) * aperture_ / 4); // how small are things in the far
            plane_translations_[idx] = ([this, w, h, reduction](typename TRenderer::RectType rc) {
                // center
                rc.x -= camera_x_ - w / 2;
                rc.y += camera_y_ - h / 2;
                // zoom
                // rc.x -= static_cast<uint32_t>(w * (1 - zoom_));
                // rc.y += static_cast<uint32_t>(h * (1 - zoom_));
                // rc.w = static_cast<uint32_t>(rc.w * zoom_);
                // rc.h = static_cast<uint32_t>(rc.h * zoom_);
                // reduce as per the distance
                rc.x += reduction * rc.w / w;
                rc.y += reduction * rc.h / h;
                rc.w -= reduction * 2 * rc.w / w;
                rc.h -= reduction * 2 * rc.h / h;
                return rc;
            });
        }
    }

    std::shared_ptr<TRenderer> renderer_;
    std::vector<std::function<typename TRenderer::RectType(typename TRenderer::RectType)>> plane_translations_;
    // System.Collections.Generic.Array<System.Func<Rect, Rect>>
    // it's a dynamic array of functions that take a Rect and return a Rect
    const units::Distance camera_z_{units::Distance::Metres(10.0)}; // camera is away 10m from the first plane
    const units::Distance distance_between_planes_{units::Distance::Metres(2.0)}; // difference between planes 
    int camera_x_;
    int camera_y_;
    double zoom_{1.0};
    const double aperture_{40.0};
    TScene const *scene_;
};
