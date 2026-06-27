#pragma once
#include "units.h"
#include <functional>
#include <memory>
#include <cstdint>

enum HDirection : std::uint8_t
{
    left,
    right
};

template <typename TCharacter>
struct HMove
{

    auto direction() const { return direction_; }

    void cancel()
    {
        *isCancelled_ = true;
    }

    HMove(HDirection direction, uint32_t start_time, units::Speed const &speed, TCharacter &character)
        : direction_{direction}, isCancelled_{std::make_shared<bool>(false)}
    {
        AddUpdate(start_time, speed, character);
    }

private:
    HDirection direction_;
    std::shared_ptr<bool> isCancelled_;
    // speed is in metres/second
    void AddUpdate(uint32_t start_time, units::Speed const &speed, TCharacter &character)
    { //  speed and character reference passing
        auto isCancelled = isCancelled_;
        if (direction_ == HDirection::left)
        {
            character.addUpdate([from = character.position_, speed, &character, start_time, isCancelled](typename TCharacter::SceneType *scene) -> auto {
                if (*isCancelled)
                {
                    return false;
                }
                int step = static_cast<int>(units::Time{scene->age() - start_time} * speed); // calculate distance
                character.position_.x = from.x - step;                      // update position
                return true;
            });
        }
        else
        {
            character.addUpdate([from = character.position_, speed, &character, start_time, isCancelled](typename TCharacter::SceneType *scene) -> auto {
                if (*isCancelled)
                {
                    return false;
                }
                int step = static_cast<int>(units::Time{scene->age() - start_time} * speed); // calculate distance
                character.position_.x = from.x + step;                      // update position
                return true;
            });
        }
    }
};

enum VDirection : std::uint8_t
{
    up,
    down
};

template <typename TCharacter>
struct VMove
{
    auto direction() const { return direction_; }

    void cancel()
    {
        *isCancelled_ = true;
    }

    VMove(VDirection direction, uint32_t start_time, units::Speed const &speed, TCharacter &character)
        : direction_{direction}, isCancelled_{std::make_shared<bool>(false)}
    {
        AddUpdate(start_time, speed, character);
    }

private:
    VDirection direction_;
    std::shared_ptr<bool> isCancelled_;

    void AddUpdate(uint32_t start_time, units::Speed const &speed, TCharacter &character)
    {
        auto isCancelled = isCancelled_;
        if (direction_ == VDirection::up)
        {
            character.addUpdate([from = character.position_, speed, &character, start_time, isCancelled](typename TCharacter::SceneType *scene) -> auto {
                if (*isCancelled)
                {
                    return false;
                }
                int step = static_cast<int>(units::Time{scene->age() - start_time} * speed);
                character.position_.y = from.y - step;
                return true;
            });
        }
        else
        {
            character.addUpdate([from = character.position_, speed, &character, start_time, isCancelled](typename TCharacter::SceneType *scene) -> auto {
                if (*isCancelled)
                {
                    return false;
                }
                int step = static_cast<int>(units::Time{scene->age() - start_time} * speed);
                character.position_.y = from.y + step;
                return true;
            });
        }
    }
};
