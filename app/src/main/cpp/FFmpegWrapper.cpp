//
// Created by jiaming.huang on 2024/6/20.
//

#include "FFmpegWrapper.h"
#include "ALOG.h"

static bool debug = false;

FFmpegWrapper::FFmpegWrapper(XData * queue)
{
    ALOGE("%s",__func__ );
    mqueue = queue;
    av_register_all();
    avformat_network_init();
    avcodec_register_all();

}
FFmpegWrapper::~FFmpegWrapper()
{
    ALOGE("%s",__func__ );
}

int FFmpegWrapper::FFmpegInit(const char* url)
{
    int ret = 0;

    ALOGE("%s",__func__ );
    ret = avformat_open_input(&fmtCtx, url, 0, 0); //打开URL文件，绑定fmtCtx
    if (ret !=0) {
        ALOGE("%s avformat_open_input open %s fail !!",__func__, url);
        return -1;
    }

    ret = avformat_find_stream_info(fmtCtx, NULL); //读取文件信息
    if ( ret !=0 ) {
        ALOGE("%s avformat_find_stream_info fail !!",__func__ );
        goto err0;
    }

    for(int i = 0; i < fmtCtx->nb_streams; i++) {
        if(fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {//获取音频流
            audioIndex = i;
            ALOGD("%s AVMEDIA_TYPE_AUDIO audioIndex = %d ",__func__ ,audioIndex);
            break;
        }
    }
    if (audioIndex < 0) {
        ALOGE("%s avformat_find_stream_info no audio stream fail !!",__func__ );
        goto err0;
    }

    innumChannels = fmtCtx->streams[audioIndex]->codecpar->channels;
    insampleRate = fmtCtx->streams[audioIndex]->codecpar->sample_rate;

    ALOGD("%s numChannels= %d sampleRate=%d !!",__func__ ,innumChannels,insampleRate);

    codecCtx = avcodec_alloc_context3(NULL);
    //stream = fmtCtx->streams[audioIndex];
    avcodec_parameters_to_context(codecCtx, fmtCtx->streams[audioIndex]->codecpar);
    codec = avcodec_find_decoder(codecCtx->codec_id);
    ALOGD("%s  numChannels= %d sampleRate=%d !!",__func__ ,innumChannels,insampleRate);

    insampleFormat = codecCtx->sample_fmt;
    insampleRate = codecCtx->sample_rate;
    ALOGD("%s numChannels= %d sampleRate=%d sampleFormat=%d !!",__func__ ,innumChannels,insampleRate,insampleFormat);

    if (avcodec_open2(codecCtx, codec, NULL) < 0) {
        ALOGE("%s avcodec_open2  fail !!",__func__ );
        goto err1;
    }

    return 0;

err1:
    avcodec_close(codecCtx);

err0:
   avformat_close_input(&fmtCtx);

   return -1;
}

int FFmpegWrapper::FFmpegRelease()
{
    ALOGE("%s",__func__ );

    isPlay = false;
    avcodec_close(codecCtx);
    avformat_close_input(&fmtCtx);

    return  0;
}

int FFmpegWrapper::FFmpegInitResample()
{
    ALOGD("%s init Resample..",__func__ );
    outnumChannels = 2;
    outsampleRate = insampleRate;
    outsampleFormat = AV_SAMPLE_FMT_S16;

    outFrameSize = outsampleRate * outnumChannels;

    swrCtx = swr_alloc();
    swrCtx = swr_alloc_set_opts(swrCtx,
                                av_get_default_channel_layout(outnumChannels),
                                (AVSampleFormat)outsampleFormat,
                                outsampleRate,

                                av_get_default_channel_layout(innumChannels),
                                (AVSampleFormat)insampleFormat,
                                insampleRate,
                                0,0);

    if (swr_init(swrCtx) < 0) {
        ALOGD("%s swr_init fail !!",__func__ );
        return -1;
    }

    return 0;
}

audioParam FFmpegWrapper::getAPara()
{
    audioParam mParam ;

    mParam.numChannels = outnumChannels;
    mParam.sampleRate = outsampleRate;
    mParam.sampleFormat = outsampleFormat;

    return mParam;
}

int FFmpegWrapper::FFmpegResample(AVFrame *indata, AVFrame *outdata)
{
    int len = 0;
    uint8_t *outBuffer = (uint8_t *) malloc(outFrameSize);

    len = swr_convert(swrCtx, &outBuffer, indata->nb_samples, (const uint8_t **)indata->data, indata->nb_samples);
    return len;

}

int FFmpegWrapper::FFmpegDecodeAudio()
{
    ALOGD("%s start decode..",__func__ );
    int ret;
    AVFrame *frame = av_frame_alloc();
    AVPacket *packet = av_packet_alloc();
    int frameIndex = 0;
    int got_frame = 0;
    long int size = 0;

    while(isPlay) {
        ret = av_read_frame(fmtCtx, packet);
        if (packet->stream_index != audioIndex)
            continue;
        if(ret < 0) {
            ALOGE("%s av_read_frame fail !!",__func__ );
            goto err0;
        }
        ret = avcodec_decode_audio4(codecCtx, frame, &got_frame,packet);
        if(ret < 0 || got_frame <= 0) {
            ALOGE("%s avcodec_decode_audio4 fail || got_frame got no frame !!",__func__ );
            goto err0;
        }
        if (debug)
            ALOGD("%s pkt_dts=%lld frame->pts=%lld packet->pts=%lld ",__func__,frame->pkt_dts, frame->pts,packet->pts);

        //样本字节数 * 单通道样本数 * 通道数
        size = av_get_bytes_per_sample((AVSampleFormat)frame->format) * frame->nb_samples * outnumChannels;
        uint8_t *outBuffer = (uint8_t *) av_malloc(size);
        swr_convert(swrCtx, &outBuffer, frame->nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
        if (debug)
            ALOGD("%s data.size=%d ",__func__, size);

        xdata data;
        data.data = (unsigned char *)outBuffer;
        data.size = size;
        //mqueue->put(data);
        mqueue->blockPut(data);

        free(outBuffer);
        frameIndex++;
    }

    ALOGE("%s start decode exit..",__func__ );
    isPlay = false;

err0:
    av_packet_unref(packet);
    av_frame_unref(frame);
    return 0;
}

void *_decodeAudio(void *args)
{
    FFmpegWrapper *p = (FFmpegWrapper *)args;

    p->FFmpegDecodeAudio();
    pthread_exit(0);

    return NULL;
}

int FFmpegWrapper::startDecode(bool playing)
{
    ALOGD("%s start play..",__func__ );
    isPlay = playing;

    pthread_create(&decodeId, NULL, _decodeAudio, this);

    return 0;
}




