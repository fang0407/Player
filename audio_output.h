#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include "SDL.h"

#include "state.h"
#include "frame_queue.hpp"
#include "av_publish_time.h"

typedef struct AudioParams
{
    int channels;
    uint64_t channel_layout;
    int sample_rate;
    enum AVSampleFormat sample_format;
    int frame_size;
} AudioParams;

class AudioOutput
{
public:
    AudioOutput();
    ~AudioOutput();

    bool Init(FrameQueue* frame_queue, const AudioParams& audio_params, AVRational time_base);

public:
    FrameQueue* frame_queue_ = NULL;
    AVRational time_base_;

    //解码后参数
    AudioParams src_tgt_;
    //SDL实际输出参数
    AudioParams dst_tgt_;

    int64_t pts_ = AV_NOPTS_VALUE;

    struct SwrContext* swr_ctx_ = NULL;
    //播放数据
    uint8_t* audio_buf_ = NULL;
    unsigned int audio_buf_size_ = 0;
    //Frame数据
    uint8_t* audio_buf1_ = NULL;
    unsigned int audio_buf1_size_ = 0;
    //实际读取位置
    int audio_buf_index_ = 0;

    AVPublishTime& av_time_ = AVPublishTime::GetInstance();
    State& state_ = State::GetInstance();
};

#endif // AUDIOOUTPUT_H
