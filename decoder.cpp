#include "decoder.h"

Decoder::Decoder()
{
}

Decoder::~Decoder()
{
    Stop();

    if (looper_.joinable())
        looper_.join();

    if (codec_ctx_) {
        avcodec_free_context(&codec_ctx_);
    }

    DEBUG("~Decoder()");
}

bool Decoder::Init(PacketQueue* packet_queue, FrameQueue* frame_queue)
{
    if (!packet_queue) {
        DEBUG("packet_queue is null");
        return false;
    }

    if (!frame_queue) {
        DEBUG("frame_queue is null");
        return false;
    }

    packet_queue_ = packet_queue;
    frame_queue_ = frame_queue;

    return true;
}

bool Decoder::Open(AVFormatContext* fmt_ctx, int stream_index)
{
    if (!fmt_ctx) {
        DEBUG("fmt_ctx is null");
        return false;
    }

    if (stream_index < 0) {
        DEBUG("stream index < 0");
        return false;
    }

    codec_ctx_ = avcodec_alloc_context3(NULL);
    if (!codec_ctx_) {
        DEBUG("avcodec_alloc_context3 failed");
        return false;
    }

    int ret = -1;
    ret = avcodec_parameters_to_context(codec_ctx_,
                                  fmt_ctx->streams[stream_index]->codecpar);
    if (ret < 0) {
        DEBUG("avcodec_parameters_to_context failed: %s", AVStrError(ret).c_str());
        return false;
    }

    AVCodec* codec = avcodec_find_decoder(codec_ctx_->codec_id);
    if (!codec) {
        DEBUG("avcodec_find_decoder failed");
        return false;
    }

    ret = avcodec_open2(codec_ctx_, codec, NULL);
    if (ret != 0) {
        DEBUG("avcodec_open2 failed :%s", AVStrError(ret).c_str());
        return false;
    }

    return true;
}

bool Decoder::Run()
{
    running_ = true;
    looper_ = std::thread(&Decoder::Loop, this);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
}

void Decoder::Stop()
{
    running_ = false;
}

bool Decoder::Loop()
{
    int ret = -1;
    auto delete_frame = [](AVFrame* frame){
        av_frame_free(&frame);
    };
    while (running_) {
        if (frame_queue_->Size() > 10) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        std::shared_ptr<AVPacket> pkt;
        packet_queue_->Pop(pkt);

        ret = avcodec_send_packet(codec_ctx_, pkt.get());
        if (ret != 0) {
            DEBUG("avcodec_send_packet failed :%s", AVStrError(ret).c_str());
            return false;
        }

        while (ret == 0) {
            AVFrame* frame = av_frame_alloc();
            ret = avcodec_receive_frame(codec_ctx_, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else if (ret != 0) {
                av_frame_free(&frame);
                DEBUG("avcodec_receive_frame failed :%s", AVStrError(ret).c_str());
                break;
            }

            std::shared_ptr<AVFrame> frame_ptr(frame, delete_frame);
            frame_queue_->Push(frame_ptr);
        }
    }

    return true;
}
