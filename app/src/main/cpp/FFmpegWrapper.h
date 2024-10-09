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
    long int getDuration();
    double getCurrentPosition();

    int FFmpegInitResample();
    int FFmpegResample(AVFrame *frame);

    int FFmpegDecodeAudio(bool playing);
    int FFmpegDemux(bool playing);

    int setPause(bool isPause);

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
    bool isPauseing = false;


    long int pktindex = 0;

    long int frame_pts = 0;

};


#endif //JMAUDIOSONIC_FFMPEGWRAPPER_H
