#ifndef VIDEOOUTPUT_H
#define VIDEOOUTPUT_H

#include <thread>

#include "state.h"
#include "frame_queue.hpp"
#include "av_publish_time.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <SDL.h>

#ifdef __cplusplus
}
#endif


class VideoOutput
{
public:
    VideoOutput();
    ~VideoOutput();

    bool Init(FrameQueue* frame_queue, int width, int height, AVRational time_base);
    void Run();

private:
    void RefreshLoop(SDL_Event* event);
    void VideoRefresh(double& remaining_time);

private:
    FrameQueue* frame_queue_;

    int video_width_;
    int video_height_;
    AVRational time_base_;
    uint32_t pixformat_ = SDL_PIXELFORMAT_IYUV;

    bool quit_ = false;

    SDL_Event event_;
    SDL_Rect rect_;
    SDL_Window* window_ = NULL;
    SDL_Renderer* renderer_ = NULL;
    SDL_Texture* texture_ = NULL;

    AVPublishTime& av_time_ = AVPublishTime::GetInstance();
    State& state_ = State::GetInstance();

};

#endif // VIDEOOUTPUT_H

