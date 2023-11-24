#ifndef STATE_H
#define STATE_H

#include <mutex>

class State
{
public:
    static State &GetInstance();

    void SetPause(bool state);
    bool IsPaused();

    void SetDisplayWidth(int width);
    void SetDisplayHeight(int height);

    int GetDisplayWidth();
    int GetDisplayHeight();

    void SetSeekTs(int64_t ts);
    int64_t GetSeekTs();

private:
    State();

private:
    std::mutex mu_;
    bool paused_ = false;
    int display_width_;
    int display_height_;

    // -1 代表不做seek 注意demuxer seek后重置-1
    int64_t ts_ = -1;
};

#endif // STATE_H
