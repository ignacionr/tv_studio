#pragma once
#include <cstdint>

namespace units
{
#define PIXELS_PER_METRE 10
    static constexpr uint32_t MetresPerKm{1000};
    static constexpr uint32_t SecondsPerHour{3600};
    static constexpr uint32_t MsPerSecond{1000};

    struct Speed
    {
        uint32_t pixels_per_second;

        Speed(uint32_t pps) : pixels_per_second{pps} { ; }
        [[nodiscard]] static auto MetresPerSecond(double metres) -> Speed { return Speed{static_cast<uint32_t>(metres * PIXELS_PER_METRE)}; };
        [[nodiscard]] static auto KmPerHour(double km) -> Speed { return Speed{static_cast<uint32_t>(km * MetresPerKm * PIXELS_PER_METRE / SecondsPerHour)}; }
    };
    struct Time
    {
        uint32_t milliseconds;
    };

    struct Distance
    {
        uint32_t pixels;

        constexpr Distance(uint32_t p) : pixels{p} { ; }
        [[nodiscard]] static constexpr auto Metres(double metres) -> Distance { return Distance{static_cast<uint32_t>(metres * PIXELS_PER_METRE)}; }
        [[nodiscard]] operator uint32_t() const { return pixels; }
        [[nodiscard]] auto meters() const -> double { return static_cast<double>(pixels) / PIXELS_PER_METRE; }
    };

    [[nodiscard]] auto operator*(Time const &t, Speed const &s) -> Distance
    {
        return Distance{t.milliseconds * s.pixels_per_second / MsPerSecond};
    }
} // namespace units
