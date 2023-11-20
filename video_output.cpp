#include "video_output.h"

VideoOutput::VideoOutput()
{

}

VideoOutput::~VideoOutput()
{
    if (texture_) {
        SDL_DestroyTexture(texture_);
    }

    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }

    if (window_) {
        SDL_DestroyWindow(window_);
    }

    SDL_Quit();
    DEBUG("~VideoOutput()");
}

bool VideoOutput::Init(FrameQueue *frame_queue, int width, int height, AVRational time_base)
{
    if (!frame_queue) {
        DEBUG("frame_queue is null");
        return false;
    }

    frame_queue_ = frame_queue;
    video_width_ = width;
    video_height_ = height;
    time_base_ = time_base;

    window_ = SDL_CreateWindow("Title",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               video_width_, video_height_,
                               SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

    if (!window_) {
        DEBUG("SDL_CreateWindow failed");
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, 0);
    if (!renderer_) {
        DEBUG("SDL_CreateRenderer failed");
        return false;
    }

    texture_ = SDL_CreateTexture(renderer_,
                                 pixformat_,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 video_width_,
                                 video_height_);


    return true;
}

void VideoOutput::Run()
{
    while (!quit_) {
        RefreshLoop(&event_);
        switch (event_.type) {
            case SDL_QUIT:
                quit_ = true;
                break;
        }
    }
}

void VideoOutput::RefreshLoop(SDL_Event *event)
{
    double remaining_time = 0.0;
    SDL_PumpEvents();
    //没有事件产生，则一直循环
    while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
        if (remaining_time > 0.0)
            std::this_thread::sleep_for(std::chrono::milliseconds(int64_t(remaining_time * 1000.0)));

        remaining_time = 0.01;
        VideoRefresh(remaining_time);
        SDL_PumpEvents();
    }
}

void VideoOutput::VideoRefresh(double &remaining_time)
{
    std::shared_ptr<AVFrame> frame;
    frame_queue_->Peek(frame);

    if (frame->format != AVPixelFormat::AV_PIX_FMT_YUV420P) {
        DEBUG("AVFrame format need convert");
        return;
    }


    double pts = frame->pts * av_q2d(time_base_);
//        std::cout << "video pts" << pts << std::endl;
    double diff =  pts - av_time_.GetClock();
//        std::cout << "video pts" << av_time_.GetClock() << std::endl;
    if (diff > 0) {
        remaining_time = std::min(diff, remaining_time);
        return;
    }

    rect_.x = 0;
    rect_.y = 0;
    rect_.w = video_width_;
    rect_.h = video_height_;

    SDL_UpdateYUVTexture(texture_, &rect_,
                         frame->data[0], frame->linesize[0],
                         frame->data[1], frame->linesize[1],
                         frame->data[2], frame->linesize[2]);

    // 清除当前显示
    SDL_RenderClear(renderer_);

    // 将纹理的数据拷贝给渲染器
    SDL_RenderCopy(renderer_, texture_, NULL, &rect_);

    // 显示
    SDL_RenderPresent(renderer_);

    frame_queue_->Pop(frame);
}
