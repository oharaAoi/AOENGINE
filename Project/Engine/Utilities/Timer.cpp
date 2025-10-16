#include "Timer.h"

bool Timer::Run(float _deltaTime) {
    timer_ += _deltaTime;
    t_ = timer_ / targetTime_;
    if (timer_ >= targetTime_) {
        return false;
    }
    return true;
}
