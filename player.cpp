#include "player.h"

Player::Player()
{

}

bool Player::Init(const std::string& file_name)
{
    bool ret = false;
    ret = demuxer_.Init(&video_packet_queue_, &audio_packet_queue_, file_name);
    if (!ret) {
        DEBUG("demuxer_.Init failed");
        return false;
    }

    ret = video_decoder_.Init(&video_packet_queue_, &video_frame_queue_);
    if (!ret) {
        DEBUG("video_decoder_.Init failed");
        return false;
    }

    ret = video_decoder_.Open(demuxer_.GetFormatContext(),
                              demuxer_.GetVideoStreamIndex());
    if (!ret) {
        DEBUG("video_decoder_.Open failed");
        return false;
    }

    ret = video_output_.Init(&video_frame_queue_,
                             demuxer_.GetVideoWidth(),
                             demuxer_.GetVideoHeight(),
                             demuxer_.GetVideoTimeBase(),
                             demuxer_.GetFormatContext());
    if (!ret) {
        DEBUG("video_output_.Init failed");
        return false;
    }

    ret = audio_decoder_.Init(&audio_packet_queue_,
                              &audio_frame_queue_);
    if (!ret) {
        DEBUG("audio_decoder_.Init failed");
        return false;
    }

    ret = audio_decoder_.Open(demuxer_.GetFormatContext(),
                              demuxer_.GetAudioStreamIndex());
    if (!ret) {
        DEBUG("audio_decoder_.Open failed");
        return false;
    }

    AudioParams audio_params;
    audio_params.channels = demuxer_.GetChannels();
    audio_params.channel_layout = demuxer_.GetChannelLayout();
    audio_params.sample_format = demuxer_.GetSampleFormat();
    audio_params.sample_rate = demuxer_.GetSampleRate();
    audio_params.frame_size = demuxer_.GetFrameSize();

    ret = audio_output_.Init(&audio_frame_queue_,
                             audio_params,
                             demuxer_.GetAudioTimeBase());
    if (!ret) {
        DEBUG("audio_output_.Init failed");
        return false;
    }

    return true;
}

bool Player::Run()
{
    bool ret = false;
    ret = demuxer_.Run();
    if (!ret) {
        DEBUG("demuxer_.Run failed");
        return false;
    }

    ret = video_decoder_.Run();
    if (!ret) {
        DEBUG("video_decoder_.Run failed");
        return false;
    }

    ret = audio_decoder_.Run();
    if (!ret) {
        DEBUG("audio_decoder_.Run failed");
        return false;
    }

    video_output_.Run();

    demuxer_.Stop();
    video_decoder_.Stop();
    audio_decoder_.Stop();

    return true;
}
