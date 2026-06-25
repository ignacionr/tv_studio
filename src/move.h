#include "units.h"
#include <functional>

enum HDirection
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
        isCancelled_ = true;
    }

    HMove(HDirection direction, uint32_t start_time, units::Speed const &speed, TCharacter &character)
        : direction_{direction}
    {
        AddUpdate(start_time, speed, character);
    }

private:
    HDirection direction_;
    bool isCancelled_{false};
    // speed is in metres/second
    void AddUpdate(uint32_t start_time, units::Speed const &speed, TCharacter &character)
    { //  speed and character reference passing
        if (direction_ == HDirection::left)
        {
            character.addUpdate([from = character.position_, speed, &character, start_time, this](typename TCharacter::SceneType *scene) {
                if (isCancelled_)
                {
                    return false;
                }
                auto step = units::Time{scene->age() - start_time} * speed; // calculate distance
                character.position_.x = from.x - step;                      // update position
                return true;
            });
        }
        else
        {
            character.addUpdate([from = character.position_, speed, &character, start_time, this](typename TCharacter::SceneType *scene) {
                if (isCancelled_)
                {
                    return false;
                }
                auto step = units::Time{scene->age() - start_time} * speed; // calculate distance
                character.position_.x = from.x + step;                      // update position
                return true;
            });
        }
    }
};
