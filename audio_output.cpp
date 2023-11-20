#include "audio_output.h"

void FillAudioPcm(void *userdata, Uint8 * stream, int len)
{
    AudioOutput* that = (AudioOutput*)userdata;
    int copy_len = 0;
    int audio_size = 0;

    while (len > 0) {
        if (that->audio_buf_size_ == that->audio_buf_index_) {
            that->audio_buf_index_ = 0;
            std::shared_ptr<AVFrame> frame;
            that->frame_queue_->Pop(frame);

            that->pts_ = frame->pts;

            //read data
            if (((frame->format != that->dst_tgt_.sample_format)
                 || (frame->sample_rate != that->dst_tgt_.sample_rate)
                 || (frame->channel_layout != that->dst_tgt_.channel_layout))
                && (!that->swr_ctx_)) {
                that->swr_ctx_ = swr_alloc_set_opts(NULL,
                                                    that->dst_tgt_.channel_layout,
                                                    (enum AVSampleFormat)that->dst_tgt_.sample_format,
                                                    that->dst_tgt_.sample_rate,
                                                    frame->channel_layout,
                                                    (enum AVSampleFormat)frame->format,
                                                    frame->sample_rate,
                                                    0, NULL);
                if (!that->swr_ctx_) {
                    DEBUG("swr_alloc_set_opts failed");
                    return;
                }

                if (swr_init(that->swr_ctx_) < 0) {
                    DEBUG("swr_init failed");
                    return;
                }
            }

            if (that->swr_ctx_) {
                const uint8_t** in = (const uint8_t**)frame->extended_data;
                uint8_t** out = &that->audio_buf1_;
                //重采样输出点数
                int out_samples = frame->nb_samples * that->dst_tgt_.sample_rate / frame->sample_rate;
                //重采样字节数
                int out_bytes = av_samples_get_buffer_size(NULL,
                                                           that->dst_tgt_.channels,
                                                           out_samples,
                                                           that->dst_tgt_.sample_format,
                                                           1);
                if (out_bytes < 0) {
                    DEBUG("av_samples_get_buffer_size failed");
                    return;
                }

                //如果that->audio_buf1_size_小于out_bytes, 重新分配内存
                av_fast_malloc(&that->audio_buf1_, &that->audio_buf1_size_, out_bytes);

                int ret = swr_convert(that->swr_ctx_, out, out_samples, in, frame->nb_samples);
                if (ret < 0) {
                    DEBUG("swr_convert failed");
                    return;
                }

                that->audio_buf_ = that->audio_buf1_;
                that->audio_buf_size_ = av_samples_get_buffer_size(NULL,
                                                                   that->dst_tgt_.channels,
                                                                   ret, //实际重采样点数
                                                                   that->dst_tgt_.sample_format,
                                                                   1);
            } else {
                audio_size = av_samples_get_buffer_size(NULL,
                                                        frame->channels,
                                                        frame->nb_samples,
                                                        (enum AVSampleFormat)frame->format,
                                                        1);
                av_fast_malloc(&that->audio_buf1_, &that->audio_buf1_size_, audio_size);
                that->audio_buf_ = that->audio_buf1_;
                that->audio_buf_size_ = audio_size;
                //对于打包格式（packed format）的音频，data 数组中的第一个指针指向整个音频帧的数据。
                memcpy(that->audio_buf_, frame->data[0], audio_size);
            }
        }

        copy_len = that->audio_buf_size_ - that->audio_buf_index_;
        if (copy_len > len) {
            copy_len = len;
        }

        //copy data to stream
        memcpy(stream, that->audio_buf_ + that->audio_buf_index_, copy_len);

        len -= copy_len;
        stream += copy_len;
        that->audio_buf_index_ += copy_len;
    }

    if (that->pts_ != AV_NOPTS_VALUE) {
        double pts = that->pts_ * av_q2d(that->time_base_);
//        std::cout << "audio pts" << pts << std::endl;
        that->av_time_.SetClock(pts);
        that->pts_ = AV_NOPTS_VALUE;
    }
}

AudioOutput::AudioOutput()
{

}

AudioOutput::~AudioOutput()
{
    SDL_PauseAudio(1);
    SDL_CloseAudio();

    if (swr_ctx_) {
        swr_free(&swr_ctx_);
    }
}

bool AudioOutput::Init(FrameQueue *frame_queue, const AudioParams& audio_params, AVRational time_base)
{
    if (!frame_queue) {
        DEBUG("frame_queue is null");
        return false;
    }

    frame_queue_ = frame_queue;
    time_base_ = time_base;
    src_tgt_ = audio_params;

    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec wanted_spec, spec;
    wanted_spec.freq = src_tgt_.sample_rate;
    wanted_spec.channels = src_tgt_.channels;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.samples = src_tgt_.frame_size;
    wanted_spec.silence = 0;
    wanted_spec.callback = FillAudioPcm;
    wanted_spec.userdata = this;

    int ret = SDL_OpenAudio(&wanted_spec, &spec);
    if (ret < 0) {
        DEBUG("SDL_OpenAudio failed");
        return false;
    }

    dst_tgt_.channels = spec.channels;
    dst_tgt_.channel_layout = av_get_default_channel_layout(dst_tgt_.channels);
    dst_tgt_.sample_rate = spec.freq;
    dst_tgt_.sample_format = AV_SAMPLE_FMT_S16;
    dst_tgt_.frame_size = src_tgt_.frame_size;

    SDL_PauseAudio(0);
    return true;
}


