#ifndef DECODER_H
#define DECODER_H

#include <thread>
#include <atomic>

#include "packet_queue.hpp"
#include "frame_queue.hpp"

class Decoder
{
public:
    Decoder();
    ~Decoder();

    bool Init(PacketQueue* packet_queue, FrameQueue* frame_queue);
    bool Run();
    void Stop();

    bool Open(AVFormatContext* fmt_ctx, int stream_index);
    bool Loop();

private:
    AVCodecContext* codec_ctx_ = NULL;

    PacketQueue* packet_queue_ = NULL;
    FrameQueue* frame_queue_ = NULL;

    std::atomic<bool> running_ = false;
    std::thread looper_;
};

#endif // DECODER_H
