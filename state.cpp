#include "state.h"

State& State::GetInstance()
{
    static State state;
    return state;
}

void State::SetPause(bool state)
{
    std::lock_guard<std::mutex> lck(mu_);
    paused_ = state;
}

bool State::IsPaused()
{
    std::lock_guard<std::mutex> lck(mu_);
    return paused_;
}

void State::SetDisplayWidth(int width)
{
    std::lock_guard<std::mutex> lck(mu_);
    display_width_ = width;
}

void State::SetDisplayHeight(int height)
{
    std::lock_guard<std::mutex> lck(mu_);
    display_height_ = height;
}

int State::GetDisplayWidth()
{
    std::lock_guard<std::mutex> lck(mu_);
    return display_width_;
}

int State::GetDisplayHeight()
{
    std::lock_guard<std::mutex> lck(mu_);
    return display_height_;
}

void State::SetSeekTs(int64_t ts)
{
    std::lock_guard<std::mutex> lck(mu_);
    ts_ = ts;
}

int64_t State::GetSeekTs()
{
    std::lock_guard<std::mutex> lck(mu_);
    return ts_;
}

State::State()
{
}
