#pragma once
#include <memory>
#include "../move.h"
#include "../units.h"

template <typename TCharacter, typename SceneType, typename TMove>
class Jump
{
private:
    enum class JumpState
    {
        ascending,
        descending,
        finished
    };

    TCharacter &subject_;
    uint32_t start_y_;
    uint32_t max_height_;
    units::Speed speed_;
    std::shared_ptr<TMove> currentMove_;
    JumpState state_{JumpState::ascending};

public:
    Jump(TCharacter &subject, units::Distance const &distance, units::Speed const &speed, uint32_t start_time)
        : subject_{subject}, speed_{speed}
    {
        start_y_ = subject.position_.y;
        max_height_ = start_y_ - distance.pixels;
        subject_.addUpdate([this](auto *scene) { return UpdateJump(scene); });
    }

    bool UpdateJump(SceneType *scene)
    {
        auto currentTime = scene->age();

        if (state_ == JumpState::ascending)
        {
            if (!currentMove_)
            {
                currentMove_ = std::make_shared<TMove>(VDirection::up, currentTime, speed_, subject_);
            }

            if (subject_.position_.y <= static_cast<int>(max_height_))
            {
                // Reached peak, cancel upward move and start falling
                currentMove_->cancel();
                currentMove_ = std::make_shared<TMove>(VDirection::down, currentTime, speed_, subject_);
                state_ = JumpState::descending;
            }
        }
        else if (state_ == JumpState::descending)
        {
            if (subject_.position_.y >= static_cast<int>(start_y_))
            {
                // Returned to start height, cancel downward move and finish
                currentMove_->cancel();
                currentMove_.reset();
                subject_.position_.y = start_y_; // Snap precisely to start height
                state_ = JumpState::finished;
            }
        }

        return state_ != JumpState::finished; // Keep updating until the state transitions to finished
    }
};
