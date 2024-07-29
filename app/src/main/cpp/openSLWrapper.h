//
// Created by jiaming.huang on 2024/6/25.
//

#ifndef JMAUDIOSONIC_OPENSLWRAPPER_H
#define JMAUDIOSONIC_OPENSLWRAPPER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include <pthread.h>
}

#include "XData.h"

class openSLWrapper {
public:
    openSLWrapper(XData * queue);
    virtual ~openSLWrapper();
    int createOpenSL(void *Param);
    int releaseOpenSL();
    void PlayCall(void *bufq);
    int startRender();
    void renderpcm();

protected:
    unsigned char *buf = 0;

private:
    XData * mqueue;
    pthread_t renderId;
    pthread_mutex_t mutex;
    pthread_cond_t notFull;
};


#endif //JMAUDIOSONIC_OPENSLWRAPPER_H
