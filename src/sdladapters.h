#pragma once
extern "C" {
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
}
#include <stdexcept>
#include <string_view>
#include <string>
#include <functional>
#include <memory>
#include <list>
#include <iostream>
#include <map>
#include <algorithm>

namespace sdl
{
    class Error : public std::runtime_error
    {
    public:
        Error() : std::runtime_error(SDL_GetError()) { ; }
    };

    class FontError : public std::runtime_error
    {
    public:
        FontError() : std::runtime_error(TTF_GetError()) { ; }
    };

    class Color
    {
    public:
        Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char opacity = SDL_ALPHA_OPAQUE)
            : value_{.r = red, .g = green, .b = blue, .a = opacity}
        {
        }
        Color(SDL_Color const &src) : value_{src} { ; }

        auto red() const { return value_.r; }
        auto green() const { return value_.g; }
        auto blue() const { return value_.g; }
        auto opacity() const { return value_.a; }

        operator SDL_Color() const { return value_; }

    private:
        SDL_Color value_;
    };

    class UsesSDL
    {
    public:
        UsesSDL()
        {
            if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
            {
                throw Error();
            }
        }
        ~UsesSDL()
        {
            SDL_Quit();
        }
    };

#pragma region Events
    class EventPump
    {
    public:
        typedef SDL_Event EventType;
        typedef std::function<bool(SDL_Event *)> HandlerType;

        void run(std::function<void()> fn, unsigned int intended_milliseconds, HandlerType handler)
        {
            SDL_Event ev;
            auto last_event{SDL_GetTicks()};
            auto available_time{intended_milliseconds};
            for (auto code = SDL_WaitEventTimeout(&ev, available_time); !code || ev.type != SDL_EventType::SDL_QUIT; code = SDL_WaitEventTimeout(&ev, available_time))
            {
                if (code)
                {
                    if (!std::any_of(handlers_.begin(), handlers_.end(), [&ev](auto const &kv) {
                            return kv.second(&ev);
                        }))
                    {
                        handler(&ev);
                    }
                }
                auto new_time = SDL_GetTicks();
                auto diff = new_time - last_event;
                if (diff < intended_milliseconds)
                {
                    available_time = intended_milliseconds - diff;
                }
                else
                {
                    fn();
                    last_event = new_time;
                    available_time = intended_milliseconds;
                }
            }
        }

        template <typename T>
        auto &operator+=(T *o)
        {
            handlers_[o] = std::bind(&T::handle_event, o, std::placeholders::_1);
            return *this;
        }

        auto &operator-=(void *o)
        {
            handlers_.erase(o);
            return *this;
        }

    private:
        std::map<void *, HandlerType> handlers_;
    };
#pragma endregion

#pragma region Surface
    class Surface
    {
    public:
        Surface(SDL_Surface *ptr) : surface_{ptr}
        {
        }
        Surface(const char *filename)
        {
            surface_ = IMG_Load(filename);
            if (!surface_)
            {
                throw Error();
            }
        }
        ~Surface()
        {
            if (surface_)
            {
                SDL_FreeSurface(surface_);
            }
        }
        operator SDL_Surface *() const { return surface_; }
        auto Dimensions() const { return SDL_Rect{.w = surface_->w, .h = surface_->h}; }

    private:
        SDL_Surface *surface_;
    };
#pragma endregion

#pragma region Texture
    class Renderer;

    class Texture
    {
    public:
        Texture(SDL_Renderer *renderer, Surface const &src)
        {
            texture_ = SDL_CreateTextureFromSurface(renderer, src);
            if (!texture_)
            {
                throw Error();
            }
        }
        ~Texture()
        {
            if (texture_)
            {
                ::SDL_DestroyTexture(texture_);
            }
        }
        operator SDL_Texture *() const { return texture_; }

        auto &SetColorMod(Uint8 r, Uint8 g, Uint8 b)
        {
            SDL_SetTextureColorMod(texture_, r, g, b);
            return *this;
        }

    private:
        SDL_Texture *texture_;
    };

#pragma endregion

#pragma region Font
    class UsesFont
    {
    public:
        UsesFont()
        {
            if (0 != TTF_Init())
            {
                throw FontError();
            }
        }
        ~UsesFont()
        {
            TTF_Quit();
        }
    };

    class Font
    {
    public:
        Font(const char *filename, int pointsize = 120)
        {
            font_ = TTF_OpenFont(filename, pointsize);
            if (NULL == font_)
            {
                throw FontError();
            }
        }
        ~Font()
        {
            if (font_)
            {
                TTF_CloseFont(font_);
            }
        }

        auto RenderText_Solid(std::string_view const &text, Color const &color) const
        {
            return std::make_shared<Surface>(TTF_RenderUTF8_Solid(const_cast<TTF_Font *>(font_), std::string(text).c_str(), color));
        }

    private:
        TTF_Font *font_;
    };
#pragma endregion
#pragma region Window
    class Renderer;
    class Window
    {
    public:
        Window(std::string_view const &title, int width, int height, EventPump &pump)
            : pump_{pump}
        {
            window_ = SDL_CreateWindow(
                std::string(title).c_str(),
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                width, height,
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            if (!window_)
            {
                throw Error();
            }
            pump += this;
        }
        ~Window()
        {
            if (window_)
            {
                SDL_DestroyWindow(window_);
            }
            pump_ -= this;
        }

        std::shared_ptr<Renderer> renderer()
        {
            if (!renderer_)
            {
                renderer_ = std::make_shared<Renderer>(*this);
            }
            return renderer_;
        }

        void AssumeClosed()
        {
            window_ = nullptr;
        }

        auto WindowID() const
        {
            return SDL_GetWindowID(window_);
        }

        bool handle_event(SDL_Event *ev)
        {
            if (0 == (ev->type & SDL_WINDOWEVENT) || ev->window.windowID != WindowID())
            {
                return false;
            }
            if (ev->window.event == SDL_WINDOWEVENT_CLOSE)
            {
                AssumeClosed();
                return true;
            }
            return false;
        }

        operator SDL_Window *()
        {
            return window_;
        }

    private:
        SDL_Window *window_;
        std::shared_ptr<Renderer> renderer_;
        EventPump &pump_;
    };
#pragma endregion
#pragma region Cursor
    class Cursor
    {
    public:
        Cursor(SDL_SystemCursor c)
            : should_free_{true}
        {
            cursor_ = ::SDL_CreateSystemCursor(c);
            if (!cursor_)
                throw Error();
        }
        Cursor()
        {
            cursor_ = ::SDL_GetCursor();
            if (!cursor_)
                throw Error();
        }
        ~Cursor()
        {
            if (cursor_ && should_free_)
            {
                ::SDL_FreeCursor(cursor_);
            }
        }

        auto &Set()
        {
            ::SDL_SetCursor(cursor_);
            return *this;
        }

    private:
        SDL_Cursor *cursor_;
        bool should_free_{false};
    };
#pragma endregion
#pragma region Renderer
    class Renderer
    {
    public:
        typedef SDL_Rect RectType;
        typedef sdl::Texture TextureType;

        static auto GetTicks()
        {
            return ::SDL_GetTicks();
        }

        Renderer(SDL_Window *window)
        {
            renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (!renderer_)
            {
                throw Error();
            }
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            SDL_RenderSetLogicalSize(renderer_, w, h);
        }
        ~Renderer()
        {
            if (renderer_)
            {
                ::SDL_DestroyRenderer(renderer_);
            }
        }

        operator SDL_Renderer *() const { return renderer_; }

        auto &GetDimensions(int *w, int *h) const
        {
            ::SDL_RenderGetLogicalSize(*this, w, h);
            return *this;
        }

        auto &Capture(std::string const &filename)
        {
            int w, h;
            GetDimensions(&w, &h);

            // Create a temporary 32-bit RGBA surface to hold the pixel buffer
            SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
            if (!surface)
            {
                throw Error();
            }

            // Read the pixels directly from the renderer's back buffer
            if (0 != SDL_RenderReadPixels(*this, nullptr, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch))
            {
                SDL_FreeSurface(surface);
                throw Error();
            }

            // Save the surface as a BMP file
            if (0 != SDL_SaveBMP(surface, filename.c_str()))
            {
                SDL_FreeSurface(surface);
                throw Error();
            }

            SDL_FreeSurface(surface);
            return *this;
        }

        auto &SetDrawColor(const Color &color)
        {
            if (0 != SDL_SetRenderDrawColor(*this, color.red(), color.green(), color.blue(), color.opacity()))
            {
                throw Error();
            }
            return *this;
        }

        auto &FillRect(const RectType *rc)
        {
            if (0 != SDL_RenderFillRect(*this, rc))
            {
                throw Error();
            }
            return *this;
        }

        auto &Clear()
        {
            SDL_RenderClear(*this);
            return *this;
        }

        auto &Clear(const Color &color)
        {
            return SetDrawColor(color).Clear();
        }

        auto &Present()
        {
            SDL_RenderPresent(*this);
            return *this;
        }

        auto &Copy(Texture const &texture, RectType const *from = nullptr, RectType const *to = nullptr)
        {
            if (0 != SDL_RenderCopy(*this, texture, from, to))
                throw Error();
            return *this;
        }

        auto &CopyEx(Texture const &texture, RectType const *from, RectType const *to,
                     double angle, SDL_Point const *center, SDL_RendererFlip flip)
        {
            if (0 != SDL_RenderCopyEx(*this, texture, from, to, angle, center, flip))
            {
                throw Error();
            }
            return *this;
        }

        auto &SetViewPort(RectType *rc)
        {
            if (0 != ::SDL_RenderSetViewport(*this, rc))
                throw Error();
            return *this;
        }

        auto &DrawLines(SDL_Point const *points, int count)
        {
            if (0 != ::SDL_RenderDrawLines(*this, points, count))
            {
                throw Error();
            }
            return *this;
        }

        auto CreateTexture(std::string const &filename, int *w = nullptr, int *h = nullptr) const
        {
            Surface surface(filename.c_str());
            auto dimensions{surface.Dimensions()};
            if (w)
                *w = dimensions.w;
            if (h)
                *h = dimensions.h;
            return std::make_shared<TextureType>(*this, surface);
        }

    private:
        SDL_Renderer *renderer_;
    };
#pragma endregion

} // namespace sdl

std::ostream &operator<<(std::ostream &out, SDL_Rect const &r)
{
    return out << "{" << r.x << "," << r.y << "," << r.w << "," << r.h << "}";
}
