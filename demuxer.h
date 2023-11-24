#ifndef DEMUXER_H
#define DEMUXER_H

#include <thread>
#include <atomic>

#include "state.h"
#include "packet_queue.hpp"

class Demuxer
{
public:
    Demuxer();
    ~Demuxer();

    bool Init(
        PacketQueue* video_packet_queue,
        PacketQueue* audio_packet_queue,
        const std::string& file_name);
    bool Run();
    void Stop();

    AVFormatContext* GetFormatContext();
    int GetVideoStreamIndex();
    int GetAudioStreamIndex();
    int GetVideoWidth();
    int GetVideoHeight();

    int GetChannels();
    uint64_t GetChannelLayout();
    enum AVSampleFormat GetSampleFormat();
    int GetSampleRate();
    int GetFrameSize();
    AVRational GetVideoTimeBase();
    AVRational GetAudioTimeBase();

private:
    bool Open();
    void Loop();

private:
    std::string file_name_;

    std::thread looper_;
    std::atomic<bool> running_ = false;

    AVFormatContext* fmt_ctx_ = NULL;
    int audio_index_ = -1;
    int video_index_ = -1;

    PacketQueue* video_packet_queue_ = NULL;
    PacketQueue* audio_packet_queue_ = NULL;

    State& state_ = State::GetInstance();
};

#endif // DEMUXER_H
