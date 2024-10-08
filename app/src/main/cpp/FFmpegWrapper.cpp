//
// Created by jiaming.huang on 2024/6/20.
//

#include "FFmpegWrapper.h"
#include "ALOG.h"

#define fftime_to_milliseconds(ts) (av_rescale(ts, 1000, AV_TIME_BASE))
#define milliseconds_to_fftime(ms) (av_rescale(ms, AV_TIME_BASE, 1000))

static int ffmpegdebug = 0;

static int dumpdecodeData(AVFrame *frame, int outnumChannels)
{
    int bytes_per_sample = 0;
    bytes_per_sample = av_get_bytes_per_sample((AVSampleFormat)frame->format);
    FILE *file = fopen("/data/outputdecode.pcm","a");
    if (!file) {
        ALOGE("%s get data is not enough..", __func__ );
        return -1;
    } else {
        for ( int i  = 0 ; i < outnumChannels ; i++) {
            fwrite(frame->data[i], 1, bytes_per_sample * frame->nb_samples, file);
        }
        fclose(file);
    }
    return 0;
}

static int dumpresampleData(void *data, int size)
{
    FILE *file = fopen("/data/outputresample.pcm","a");
    if (!file) {
        ALOGE("%s get data is not enough size:%d  ..", __func__ ,size);
        return -1;
    } else {
        fwrite(data, 1, size, file);
        fclose(file);
    }
    return 0;
}

FFmpegWrapper::FFmpegWrapper(XData * queue)
{
    ALOGE("%s",__func__ );
    mqueue = queue;
    mpktQueue = (Queue *)malloc(sizeof (Queue));
    if (mpktQueue == NULL) {
        ALOGE("%s malloc queue fail ",__func__ );
        return ;
    }
    memset(mpktQueue, 0 , sizeof (Queue));
    initQueue(mpktQueue);

    av_register_all();
    avformat_network_init();
    avcodec_register_all();

}
FFmpegWrapper::~FFmpegWrapper()
{
    ALOGE("%s",__func__ );
    destroyQueue(mpktQueue);
    free(mpktQueue);

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

    codecCtx = avcodec_alloc_context3(NULL);
    //stream = fmtCtx->streams[audioIndex];
    avcodec_parameters_to_context(codecCtx, fmtCtx->streams[audioIndex]->codecpar);
    codec = avcodec_find_decoder(codecCtx->codec_id);

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

int FFmpegWrapper::FFmpegResample(AVFrame *frame)
{
    xdata mxdata;
    int size = 0;
    long int frameIndex = 0;

    //样本字节数 * 单通道样本数 * 通道数
    size = av_get_bytes_per_sample((AVSampleFormat)frame->format) * frame->nb_samples * outnumChannels;
    mxdata.data = mqueue->AllocFrameBuffer(size);
    mxdata.frameindex = frameIndex++;
    mxdata.size = size;
    //uint8_t * outBuffer = (uint8_t *) av_malloc(size);
    swr_convert(swrCtx, &mxdata.data, frame->nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
    //free(outBuffer);

    if (ffmpegdebug > 2) {
        dumpresampleData(mxdata.data, mxdata.size);
        ALOGI("%s blockPut frameindex=%ld data.size=%d \n",__func__, frameIndex, size);
    }
    mqueue->blockPut(mxdata);

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

long int FFmpegWrapper::getDuration()
{
    int64_t duration = 0;

    if (!fmtCtx) {
        ALOGE("%s fmtCtx = %p ！！", __func__, fmtCtx );
        return -1;
    }

    duration = fftime_to_milliseconds(fmtCtx->duration);
    //if (ffmpegdebug > 3)
         ALOGE("%s duration1 = %lld duration2 = %lld！！", __func__, fmtCtx->duration , duration );
    return duration;
}

double FFmpegWrapper::getCurrentPosition() {
    int64_t startPos;
    int64_t startDiff = 0;
    int64_t curPos = 0;
    int64_t curDiff = 0;

    if (!fmtCtx) {
        ALOGE("%s fmtCtx = %p ！！", __func__, fmtCtx);
        return -1;
    }

    startPos = fmtCtx->start_time;
    if (startPos > 0 && startPos != AV_NOPTS_VALUE)
        startDiff = fftime_to_milliseconds(startPos);

    curPos = frame_pts;

    curDiff = fftime_to_milliseconds(curPos);

    if (curPos < 0 || curPos < startPos)
        return 0;

    // int64_t adjPos = curDiff - startDiff;
    int64_t adjPos = curPos - startPos;
    double ptsTime = adjPos * av_q2d(fmtCtx->streams[audioIndex]->time_base);

    //if (ffmpegdebug > 3) {
        ALOGD("%s startPos = %lld startDiff=%lld curPos=%lld curDiff = %lld adjPos=%lld !! \n",
              __func__, startPos, startDiff, curPos, curDiff, adjPos);
        ALOGD("%s position ptsTime = %f !! \n", __func__, ptsTime);
   //}

    return ptsTime;
}

int FFmpegWrapper::FFmpegDecodeAudio(bool playing)
{
    ALOGD("%s start decode..",__func__ );
    int ret;
    AVFrame *frame = av_frame_alloc();
    AVPacket *packet;
    long int decodedpktindex = 0;
    int got_frame = 0;
    isPlay = playing;

    while(isPlay || (pktindex - decodedpktindex) != 0 ) {
       if (isPauseing){
            XSleep(1);
            continue;
        }

        // get packet data from pktqueue;
        packet = (AVPacket *)dequeue(mpktQueue, &decodedpktindex);

        int consumed  = 0;
        while (packet->size > 0) {
            consumed = avcodec_decode_audio4(codecCtx, frame, &got_frame,packet);
            //if(consumed < 0 ) {
            //    ALOGE("%s consumed < 0 !!",__func__ );
            //    break;
            //}
            if( got_frame <= 0) {
                ALOGE("%s avcodec_decode_audio4 fail || got_frame got no frame !!",__func__ );
                goto err0;
            }
            if (ffmpegdebug > 3) {
                dumpdecodeData(frame,  outnumChannels);
                ALOGI("%s pktindex =%d packet->size=%d consumed=%d got_frame=%d \n",
                      __func__, decodedpktindex, packet->size,consumed,got_frame);
                ALOGI("%s frame->pkt_dts=%lld  frame->pts=%lld packet->pts=%lld ",
                      __func__,frame->pkt_dts, frame->pts,packet->pts);
            }
            frame_pts = frame->pts;

            FFmpegResample(frame);

            packet->data += consumed;
            packet->size -= consumed;
        }

        if (packet) {
            if (ffmpegdebug > 3) ALOGE("%s delete decoded pktindex =%ld",__func__,decodedpktindex );
            free(packet);
        }
       // av_packet_free((AVPacket **) &packet);
    }

    ALOGD("%s start decode exit..",__func__ );
    isPlay = false;

    return 0;
err0:
    ALOGE("%s start decode err0 exit..",__func__ );
    av_frame_unref(frame);
    return -1;
}

int FFmpegWrapper::FFmpegDemux(bool playing)
{
    int ret = -1;
    AVPacket *packet = av_packet_alloc();

    isPlay = playing;

    while(isPlay) {
        if (isPauseing){
            XSleep(1);
            continue;
        }

        ret = av_read_frame(fmtCtx, packet);
        if (ret < 0) {
            ALOGE("%s av_read_frame fail !!", __func__);
            goto err0;
        }

        if (packet->stream_index == audioIndex) {
            // put packet data to pktqueue;
            void *pktdata = malloc(sizeof(AVPacket));
            memcpy(pktdata, packet, sizeof(AVPacket));
            pktindex++;
            if (ffmpegdebug > 3) ALOGD("%s  pktindex =%d packet->size=%d ", __func__,pktindex,packet->size);
            enqueue(mpktQueue, (void *) pktdata, pktindex);

        } else {
            continue;
        }
    }

    av_packet_free((AVPacket **) &packet);
    return ret;

err0:
    av_packet_unref(packet);
    return -1;
}



int FFmpegWrapper::FFmpegRelease()
{
    ALOGE("%s",__func__ );

    isPlay = false;
    avcodec_close(codecCtx);
    avformat_close_input(&fmtCtx);

    ALOGE("%s end",__func__ );
    return  0;
}

int FFmpegWrapper::setPause(bool isPause)
{
    isPauseing = isPause;
    ALOGD("%s isPauseing = %d  \n",__func__,isPauseing );
    return 0;
}

