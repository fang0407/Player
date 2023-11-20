#include "demuxer.h"

Demuxer::Demuxer()
{
}

Demuxer::~Demuxer()
{
    Stop();

    if (looper_.joinable())
        looper_.join();

    if (fmt_ctx_) {
        avformat_close_input(&fmt_ctx_);
    }

    DEBUG("~Demuxer()");
}

bool Demuxer::Init(
    PacketQueue* video_packet_queue,
    PacketQueue* audio_packet_queue,
    const std::string &file_name)
{
    if (!video_packet_queue) {
        DEBUG("video_packet_queue is null");
        return false;
    }

    if (!audio_packet_queue) {
        DEBUG("audio_packet_queue is null");
        return false;
    }

    file_name_ = file_name;
    video_packet_queue_ = video_packet_queue;
    audio_packet_queue_ = audio_packet_queue;

    bool ret = Open();
    if (!ret) {
        DEBUG("Open() failed");
        return false;
    }
    return true;
}

bool Demuxer::Run()
{
    running_ = true;
    looper_ = std::thread(&Demuxer::Loop, this);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
}

void Demuxer::Stop()
{
    running_ = false;
}

AVFormatContext *Demuxer::GetFormatContext()
{
    return fmt_ctx_;
}

int Demuxer::GetVideoStreamIndex()
{
    return video_index_;
}

int Demuxer::GetAudioStreamIndex()
{
    return audio_index_;
}

int Demuxer::GetVideoWidth()
{
    if (fmt_ctx_ && video_index_ != -1) {
        return fmt_ctx_->streams[video_index_]->codecpar->width;
    }
    return 0;
}

int Demuxer::GetVideoHeight()
{
    if (fmt_ctx_ && video_index_ != -1) {
        return fmt_ctx_->streams[video_index_]->codecpar->height;
    }
    return 0;
}

int Demuxer::GetChannels()
{
    if (fmt_ctx_ && audio_index_ != -1) {
        return fmt_ctx_->streams[audio_index_]->codecpar->channels;
    }
    return 0;
}

uint64_t Demuxer::GetChannelLayout()
{
    if (fmt_ctx_ && audio_index_ != -1) {
        return fmt_ctx_->streams[audio_index_]->codecpar->channel_layout;
    }
    return 0;
}

enum AVSampleFormat Demuxer::GetSampleFormat()
{
    if (fmt_ctx_ && audio_index_ != -1) {
        return (enum AVSampleFormat)fmt_ctx_->streams[audio_index_]->codecpar->format;
    }
    return AVSampleFormat::AV_SAMPLE_FMT_NONE;
}

int Demuxer::GetSampleRate()
{
    if (fmt_ctx_ && audio_index_ != -1) {
        return fmt_ctx_->streams[audio_index_]->codecpar->sample_rate;
    }
    return 0;
}

int Demuxer::GetFrameSize()
{
    if (fmt_ctx_ && audio_index_ != -1) {
        return fmt_ctx_->streams[audio_index_]->codecpar->frame_size;
    }
    return 0;
}

AVRational Demuxer::GetVideoTimeBase()
{
    if (fmt_ctx_ && video_index_ != -1) {
        return fmt_ctx_->streams[video_index_]->time_base;
    }

    return AVRational{0, 0};
}

AVRational Demuxer::GetAudioTimeBase()
{
    if (fmt_ctx_ && audio_index_ != -1) {
        return fmt_ctx_->streams[audio_index_]->time_base;
    }

    return AVRational{0, 0};
}

bool Demuxer::Open()
{
    int ret = -1;
    fmt_ctx_ = avformat_alloc_context();
    if (!fmt_ctx_) {
        DEBUG("avformat_alloc_context alloc failed.");
        return false;
    }

    ret = avformat_open_input(&fmt_ctx_, file_name_.c_str(), NULL, NULL);
    if (ret != 0) {
        DEBUG("avformat_open_input failed: %s; file name:%s", AVStrError(ret).c_str(), file_name_.c_str());
        return false;
    }

    ret = avformat_find_stream_info(fmt_ctx_, NULL);
    if (ret < 0) {
        DEBUG("avformat_find_stream_info failed: %s", AVStrError(ret).c_str());
        return false;
    }

    av_dump_format(fmt_ctx_, 0, file_name_.c_str(), 0);

    video_index_ = av_find_best_stream(fmt_ctx_, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    audio_index_ = av_find_best_stream(fmt_ctx_, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);

    if (video_index_ < 0 || audio_index_ < 0) {
        DEBUG("not found video or audio stream");
        return false;
    }

    return true;
}

void Demuxer::Loop()
{
    int ret = -1;
    auto delete_packet = [](AVPacket* pkt) {
        av_packet_free(&pkt);
    };

    while (running_) {
        if (video_packet_queue_->Size() > 100 || audio_packet_queue_->Size() > 100) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        AVPacket* pkt = av_packet_alloc();
        ret = av_read_frame(fmt_ctx_, pkt);
        if (ret != 0) {
            av_packet_free(&pkt);
            DEBUG("av_read_frame failed: %s", AVStrError(ret).c_str());
            break;
        }

        std::shared_ptr<AVPacket> pkt_ptr(pkt, delete_packet);

        if (pkt_ptr->stream_index == AVMEDIA_TYPE_AUDIO) {
            audio_packet_queue_->Push(pkt_ptr);
        } else if (pkt_ptr->stream_index == AVMEDIA_TYPE_VIDEO) {
            video_packet_queue_->Push(pkt_ptr);
        }
    }
}
