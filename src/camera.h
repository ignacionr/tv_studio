#pragma once
#include <memory>
#include <algorithm>
#include <string>
#include <iomanip>
#include <sstream>
#include <utility>
#include "units.h"
#include "sdladapters.h"

// A Camera produces renders by superposing the planes of a scene, from a perspective
// camera is a class has two parameters of render and scene
template <typename TRenderer, typename TScene>
struct Camera
{
    Camera(std::shared_ptr<TRenderer> renderer, bool show_overlay = false) // constructor by passing render shared_ptr
        // Educational comment: We initialize all member variables in the member initialization list.
        // This is a strict C++ best practice that prevents compiler warnings/errors about uninitialized objects,
        // and guarantees that variables like camera_x_, camera_y_, and the scene_ pointer have safe default values
        // before any member function executes.
        : renderer_(std::move(renderer)), target_character_(nullptr), font_{nullptr}, show_overlay_{show_overlay} { ; }

    auto scene(TScene const *scene_param) -> void // setting up scene to the camera
    {
        scene_ = scene_param;
        plane_translations_.resize(0); // why it resize to zero? initial vector size is not zero?
        camera_y_ = scene_param->pixel_size().h / 2; 
    }

    auto follow(typename TScene::CharacterType const *character) -> void // set character to follow
    {
        target_character_ = character;
    }

    auto configure_capture(uint32_t start_ms, uint32_t end_ms, uint32_t fps) -> void // NOLINT(bugprone-easily-swappable-parameters)
    {
        capture_start_ms_ = start_ms;
        capture_end_ms_ = end_ms;
        capture_fps_ = fps;
        capture_enabled_ = true;
    }

    auto render() -> void
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

        // Render the camera position text overlay in the upper left corner if enabled
        if (show_overlay_)
        {
            render_position_overlay();
        }

        // Capture frame if capture is enabled and current time falls within target window.
        // We declare local tracking variables as 'const' with brace-initializers '{}'
        // to prevent accidental mutability and show students clean C++ practices.
        if (capture_enabled_ && scene_)
        {
            const uint32_t current_time{scene_->age()};
            if (current_time >= capture_start_ms_ && current_time <= capture_end_ms_)
            {
                const uint32_t target_time{capture_start_ms_ + ((next_capture_index_ * 1000) / capture_fps_)};
                if (current_time >= target_time)
                {
                    std::ostringstream ss;
                    ss << "frame_" << std::setfill('0') << std::setw(4) << next_capture_index_ << ".png";
                    try
                    {
                        renderer_->Capture(ss.str());
                        next_capture_index_++;
                    }
                    catch (...)
                    {
                        // Ignore capture errors
                    }
                }
            }
        }

        renderer_->Present(); // SDL requirement
    }

    auto update() -> void
    {
        // camera_z_ = std::min({(-50) + (static_cast<int>(scene_->age()) / 100), -10});
        if (target_character_)
        {
            camera_x_ = target_character_->position_.x + (target_character_->position_.w / 2);
        }
        else
        {
            camera_x_ = 0;
        }
        // zoom_ = scene_->age() * 0.00001 + 1.0;
        create_plane_translations();
    }

// C++ style guide allows private member variables at the end
private:
    auto create_plane_translations() -> void
    {
        const int planes = scene_->size();
        plane_translations_.resize(planes); // does this change at runtime?
        int w, h;
        renderer_->GetDimensions(&w, &h); // out parameters ? why pass the address?

        // Educational comment: We dynamically find which plane (depth layer) contains the followed character.
        // That plane will serve as our "focus plane" (scrolling exactly 1:1 with the camera).
        // Planes closer to the camera will scroll faster, and planes further away will scroll slower.
        int target_idx = 2; // Default to middle plane (plane 2) if no character is followed
        if (target_character_ && scene_)
        {
            for (int idx = 0; idx < planes; ++idx)
            {
                auto plane = scene_->at(idx);
                for (auto const &ch : *plane)
                {
                    if (ch.get() == target_character_)
                    {
                        target_idx = idx;
                        break;
                    }
                }
            }
        }

        for (int idx = 0; idx < planes; ++idx)
        {
            auto reduction = static_cast<int>(log(
                (camera_z_ + (distance_between_planes_ * idx)) / aperture_) * (aperture_ / 4)); // how small are things in the far
            
            // Educational comment: We calculate a relative parallax factor using the perspective depth ratio.
            // factor = Z_focus / Z_current.
            // Since Z_focus = camera_z_ + d * target_idx, and Z_current = camera_z_ + d * idx:
            double factor = static_cast<double>(camera_z_ + (distance_between_planes_ * target_idx)) / 
                            static_cast<double>(camera_z_ + (distance_between_planes_ * idx));

            plane_translations_[idx] = ([this, w, h, reduction, factor](typename TRenderer::RectType rc) -> auto {
                // center with parallax scroll factor
                rc.x -= static_cast<int>((camera_x_ - (w / HalfDivisor)) * factor);
                rc.y += camera_y_ - (h / 2);
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

    auto render_position_overlay() -> void
    {
        if (!font_)
        {
            try
            {
                // Educational comment: std::make_unique is a helper function to create a std::unique_ptr.
                // It avoids direct use of raw 'new' expressions, enforcing standard RAII principles.
                font_ = std::make_unique<sdl::Font>("rsrc/fonts/Arial.ttf", OverlayFontSize);
            }
            catch (...)
            {
                return;
            }
        }

        if (font_)
        {
            std::string text{"Camera - X: " + std::to_string(camera_x_) + ", Y: " + std::to_string(camera_y_)};
            sdl::Color white_color{FullIntensity, FullIntensity, FullIntensity, FullIntensity};
            sdl::Color bg_color{0, 0, 0, OverlayBgOpacity}; // Semi-transparent black background
            try
            {
                auto surface = font_->RenderText_Solid(text, white_color);
                if (surface)
                {
                    // Create an SDL texture from the text surface.
                    sdl::Texture text_texture(*renderer_, *surface);
                    auto dims = surface->Dimensions();

                    // Position the overlay with a 10px margin.
                    typename TRenderer::RectType bg_rect{OverlayPadding, OverlayPadding, dims.w + OverlayExtraWidth, dims.h + OverlayPadding};
                    renderer_->SetDrawColor(bg_color);
                    renderer_->FillRect(&bg_rect);

                    typename TRenderer::RectType dest{OverlayTextX, OverlayTextY, dims.w, dims.h};
                    renderer_->Copy(text_texture, nullptr, &dest);
                }
            }
            catch (...)
            {
                // Gracefully ignore text rendering failures
            }
        }
    }

    static constexpr int OverlayFontSize{18};
    static constexpr unsigned char FullIntensity{255};
    static constexpr unsigned char OverlayBgOpacity{180};
    static constexpr int OverlayPadding{8};
    static constexpr int OverlayExtraWidth{12};
    static constexpr int OverlayTextX{14};
    static constexpr int OverlayTextY{12};
    static constexpr uint32_t DefaultCaptureFps{24};
    static constexpr double HalfDivisor{2.0};

    std::shared_ptr<TRenderer> renderer_;
    std::vector<std::function<typename TRenderer::RectType(typename TRenderer::RectType)>> plane_translations_;
    // System.Collections.Generic.Array<System.Func<Rect, Rect>>
    // it's a dynamic array of functions that take a Rect and return a Rect
    const units::Distance camera_z_{units::Distance::Metres(10.0)}; // camera is away 10m from the first plane
    const units::Distance distance_between_planes_{units::Distance::Metres(2.0)}; // difference between planes 
    int camera_x_{0};
    int camera_y_{0};
    double zoom_{1.0};
    const double aperture_{40.0};
    TScene const *scene_{nullptr};
    typename TScene::CharacterType const *target_character_;
    bool show_overlay_;
    bool capture_enabled_{false};
    uint32_t capture_start_ms_{0};
    uint32_t capture_end_ms_{0};
    uint32_t capture_fps_{DefaultCaptureFps};
    uint32_t next_capture_index_{0};
    
    // Educational comment: We use std::unique_ptr to manage the font resource lifetime.
    // std::unique_ptr enforces single ownership and guarantees that the TTF_Font resource
    // is closed properly (via TTF_CloseFont) in Font's destructor when this pointer goes out of scope.
    std::unique_ptr<sdl::Font> font_;
};
