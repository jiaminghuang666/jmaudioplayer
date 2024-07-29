//
// Created by jiaming.huang on 2024/6/21.
//

#ifndef JMAUDIOSONIC_JMAUDIOPLAYER_H
#define JMAUDIOSONIC_JMAUDIOPLAYER_H

#include "FFmpegWrapper.h"
#include "openSLWrapper.h"


class jmAudioPlayer {
public:
    jmAudioPlayer();
    virtual ~jmAudioPlayer();

    static jmAudioPlayer * getInstant()
    {
        static jmAudioPlayer mjmAudioPlayer;
        return &mjmAudioPlayer;
    }

     int createjmAudioPlayer(const char *Url);
     int releasejmAudioPlayer();
     int play();

private:
    FFmpegWrapper * mffmpeg ;
    openSLWrapper * mopenSl ;
    XData * queue;
};


#endif //JMAUDIOSONIC_JMAUDIOPLAYER_H
