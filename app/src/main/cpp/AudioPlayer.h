//
// Created by jiaming.huang on 2024/6/21.
//

#ifndef JMAUDIOSONIC_JMAUDIOPLAYER_H
#define JMAUDIOSONIC_JMAUDIOPLAYER_H

#include "FFmpegWrapper.h"
#include "openSLWrapper.h"


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
    int start();
    int stop();
    int pause();
    int seek();
    double getCurrentPosition();
    long int getDuration();
    int getParam(int id, void *param);

private:
    int postEvent(int id,int arg1,int arg2);

    FFmpegWrapper * mffmpeg ;
    openSLWrapper * mopenSl ;
    XData * queue;
    const char *myUrl;
};


#endif //JMAUDIOSONIC_JMAUDIOPLAYER_H
