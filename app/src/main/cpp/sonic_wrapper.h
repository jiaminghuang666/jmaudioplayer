//
// Created by jiaming.huang on 2024/6/18.
//

#ifndef JMAUDIOSONIC_SONIC_WRAPPER_H
#define JMAUDIOSONIC_SONIC_WRAPPER_H

#include "sonic/sonic.h"
#include "ALOG.h"
#include "androidAudioUntil.h"



typedef struct audioParam {
    sonicStream stream;
    int numChannels;
    int format;
}audioParam;

class sonic_wrapper {
private:
    static sonic_wrapper * instance ;

public:
    sonic_wrapper();
    virtual ~sonic_wrapper();
    static sonic_wrapper * getInstance()
    {
        if (instance == NULL) {
            instance = new sonic_wrapper();
        }
        return instance;
    }
    static void destroyInstance(){
        delete instance;
        instance = NULL;
        return;
    }

    virtual int createSonic(void *mhandle,int sampleRate,int numChannels,float speed);
    virtual int writeData2Sonic(void *mhandle, void *buf, size_t in_size);
    virtual int readData4Sonic(void *mhandle, void *outBuffer,size_t out_size);
    virtual int releaseSonic(void *mhandle);

};



#endif //JMAUDIOSONIC_SONIC_WRAPPER_H
