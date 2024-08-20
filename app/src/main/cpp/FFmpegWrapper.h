//
// Created by jiaming.huang on 2024/6/20.
//

#ifndef JMAUDIOSONIC_FFMPEGWRAPPER_H
#define JMAUDIOSONIC_FFMPEGWRAPPER_H

#include "XData.h"
#include "FFmpegqueue.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>

#include <pthread.h>
}


struct AVFormatContext;
struct AVCodecContext;
struct AVStream ;
struct AVCodec;
struct SwrContext;

class FFmpegWrapper  {
public:
    FFmpegWrapper(XData * queue);
    virtual ~FFmpegWrapper();

    int FFmpegInit(const char* url);
    int FFmpegRelease();

    audioParam getAPara();
    long getDuration();

    int FFmpegInitResample();

    int FFmpegDecodeAudio();
    int FFmpegDemux();

    int startDecode(bool playing);
    int startDemux(bool playing);
    //std::vector<AVFrame *> queue;

private:

    XData * mqueue;
    Queue * mpktQueue;

    AVFormatContext *fmtCtx = 0;
    AVCodecContext  *codecCtx = 0;
    //AVStream *stream = 0;
    AVCodec  *codec = 0;
    SwrContext *swrCtx = 0;


    int audioIndex = -1;
    int innumChannels = 0;
    int insampleRate = 0;
    int insampleFormat = 0;

    int outnumChannels = 0;
    int outsampleRate = 0;
    int outsampleFormat = 0;
    int outFrameSize = 0;

    bool isPlay = false;
    pthread_t decodeId;
    pthread_t demuxId;

};


#endif //JMAUDIOSONIC_FFMPEGWRAPPER_H
