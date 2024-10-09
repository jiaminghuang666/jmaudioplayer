//
// Created by jiaming.huang on 2024/6/21.
//

#ifndef JMAUDIOSONIC_JMAUDIOPLAYER_H
#define JMAUDIOSONIC_JMAUDIOPLAYER_H

#include "FFmpegWrapper.h"
#include "openSLWrapper.h"



static bool playing;

enum paramId {
    PARAM_DURATION = 0,
    PARAM_POSITION,
    PARAM_MAX
};

class jmAudioPlayer {
public:
    jmAudioPlayer();
    virtual ~jmAudioPlayer();

    static jmAudioPlayer * getInstant()
    {
        static jmAudioPlayer mjmAudioPlayer;
        return &mjmAudioPlayer;
    }

    int setdataSource(const char *Url);
    int prepareAsync();
    int release();

    int start();
    int stop();
    int pause(bool isPause);
    int seek();
    double getCurrentPosition();
    long int getDuration();
    int getParam(int id, void *param);

    int startDecode();
    int startDemux();

private:
    FFmpegWrapper * mffmpeg ;
    openSLWrapper * mopenSl ;
    XData * queue;
    const char *myUrl;

    pthread_t decodeId;
    pthread_t demuxId;
};


#endif //JMAUDIOSONIC_JMAUDIOPLAYER_H
