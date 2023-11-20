#ifndef PLAYER_H
#define PLAYER_H

#include "demuxer.h"
#include "decoder.h"
#include "video_output.h"
#include "audio_output.h"

class Player
{
public:
    Player();
    bool Init(const std::string& file_name);
    bool Run();

private:
    Demuxer demuxer_;
    Decoder video_decoder_;
    Decoder audio_decoder_;
    VideoOutput video_output_;
    AudioOutput audio_output_;

    PacketQueue audio_packet_queue_;
    PacketQueue video_packet_queue_;

    FrameQueue audio_frame_queue_;
    FrameQueue video_frame_queue_;
};

#endif // PLAYER_H
