#include "Timer.h"
#include <algorithm>

using namespace AOENGINE;

bool Timer::Run(float _deltaTime) {
    timer_ += _deltaTime;
    t_ = timer_ / targetTime_;
    t_ = std::clamp(t_, 0.0f, 1.0f);
    if (timer_ > targetTime_) {
        return false;
    }
    return true;
}
