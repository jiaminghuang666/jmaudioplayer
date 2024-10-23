//
// Created by jiaming.huang on 2024/6/21.
//

#ifndef JMAUDIOSONIC_JMAUDIOPLAYER_H
#define JMAUDIOSONIC_JMAUDIOPLAYER_H

#include "FFmpegWrapper.h"
#include "openSLWrapper.h"

static bool playing;

enum MSG {
    MSG_START = 0,
    MSG_ERROR = 100,
    MSG_INFO = 200,
    MSG_EOS   = 300,
};

enum paramId {
    PARAM_MEDIAINFO = 0,
    PARAM_AUDIOTRACK = 100,
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
    int start();
    int stop();
    int pause(bool isPause);
    int seek();
    double getCurrentPosition();
    long int getDuration();
    int getParam(int id, void *param);

    int startDecode();
    int startDemux();

    int postEvent(int id,int arg1,int arg2);

private:
    FFmpegWrapper * mffmpeg ;
    openSLWrapper * mopenSl ;
    XData * queue;
    const char *myUrl;

    pthread_t decodeId;
    pthread_t demuxId;

};


#endif //JMAUDIOSONIC_JMAUDIOPLAYER_H
