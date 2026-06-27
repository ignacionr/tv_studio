#pragma once
#include <cstdint>

namespace units
{
#define PIXELS_PER_METRE 10
    struct Speed
    {
        uint32_t pixels_per_second;

        Speed(uint32_t pps) : pixels_per_second{pps} { ; }
        [[nodiscard]] static Speed MetresPerSecond(double metres) { return Speed{static_cast<uint32_t>(metres * PIXELS_PER_METRE)}; };
        [[nodiscard]] static Speed KmPerHour(double km) { return Speed{static_cast<uint32_t>(km * 1000 * PIXELS_PER_METRE / 3600)}; }
    };
    struct Time
    {
        uint32_t milliseconds;
    };

    struct Distance
    {
        uint32_t pixels;

        constexpr Distance(uint32_t p) : pixels{p} { ; }
        [[nodiscard]] static constexpr Distance Metres(double metres) { return Distance{static_cast<uint32_t>(metres * PIXELS_PER_METRE)}; }
        [[nodiscard]] operator uint32_t() const { return pixels; }
        [[nodiscard]] double meters() const { return static_cast<double>(pixels) / PIXELS_PER_METRE; }
    };

    [[nodiscard]] Distance operator*(Time const &t, Speed const &s)
    {
        return Distance{t.milliseconds * s.pixels_per_second / 1000};
    }
} // namespace units
