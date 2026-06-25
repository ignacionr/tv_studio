#include <functional>
#include <memory>
#include <map>

template <typename TCharacter, typename SceneType, typename TMove>
class Prosecution
{
private:
    TCharacter &subject_;
    TCharacter const &target_;
    bool isEndOnCollision_;
    std::shared_ptr<TMove> currentMove_;
    uint32_t lastUpdate_{0};
    std::map<HDirection, std::function<void()>> setupDirections_;

public:
    Prosecution(TCharacter &subject, const TCharacter &target, bool isEndOnCollision, std::map<HDirection, std::function<void()>> setupDirections)
    : subject_{subject}, target_{target}, isEndOnCollision_{isEndOnCollision}, setupDirections_{setupDirections} // initialization list, when we have reference we have to use it (references need to be initialize)
    {
        // "duck typing": "if it looks like a duck, sounds like a duck, walks like a duck, then it's a duck"
        subject_.addUpdate([this](auto *scene) { return UpdateSubject(scene); });
        // nowadays, we don't use "bind" but prefer a lambda like that before
        // _subject.addUpdate(std::bind(&Prosecution<TCharacter,SceneType>::UpdateSubject, this, std::placeholders::_1));
    }

    bool UpdateSubject(SceneType *scene)
    {
        // we need to follow the target, two options:
        // 1. we make the position of our subject closer to the target, or
        // 2. we create and maintain a HMove that will go towards the target

        // the character's level of dispersion will decide how fast we update the moves
        auto dispersionCount = subject_.dispersionCount();

        // when it's time to update a move, we will cancel the outstanding move and create a new one
        auto currentTime = scene->age();
        auto keep_will{true};
        if ((currentTime - lastUpdate_) > dispersionCount)
        {
            auto direction{subject_.position_.x < target_.position_.x ? HDirection::right : HDirection::left};
            if (currentMove_)
            {
                if (currentMove_->direction() != direction)
                {
                    currentMove_->cancel();
                    currentMove_.reset();
                    keep_will = !isEndOnCollision_;
                }
            }
            else
            {
                auto factory{subject_.moveFactory()};
                auto setupPos {setupDirections_.find(direction)};
                if (setupPos != setupDirections_.end()) {
                    setupPos->second();
                }
                currentMove_ = factory(direction, currentTime);
            }
            lastUpdate_ = currentTime;
        }
        return keep_will;
    }
};
