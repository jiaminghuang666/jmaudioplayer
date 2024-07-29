//
// Created by jiaming.huang on 2024/6/19.
//

#ifndef JMAUDIOSONIC_JMSONICIMP_H
#define JMAUDIOSONIC_JMSONICIMP_H

#include "sonic_wrapper.h"

sonic_wrapper* sonic_wrapper::instance = NULL;

class jmSonicImp {
public:
    jmSonicImp();
    ~jmSonicImp();

    virtual int jmSonicOpen(float speed,int sampleRate,int numChannels,int format);
    virtual int jmSonicClose();
    virtual int jmSonicProcess(void *inBuffer, size_t *in_size,void *outBuffer,size_t *out_size );

private:
    sonic_wrapper* sonicWrapper = NULL;
    audioParam* handle = NULL;
};


#endif //JMAUDIOSONIC_JMSONICIMP_H
