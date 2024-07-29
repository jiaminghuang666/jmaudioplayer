//
// Created by jiaming.huang on 2024/6/19.
//

#include "jmSonicImp.h"
#include "ALOG.h"
#include "androidAudioUntil.h"

extern "C" {
#include <cstdlib>
#include <cstring>
#include <jni.h>

};

#define MINUM_SPEED_OUTPUT_FRAMES 512

int jmSonicImp::jmSonicOpen(float speed,int sampleRate,int numChannels,int format)
{
    int ret = -1;

    sonicWrapper = sonic_wrapper::getInstance();

    handle = (audioParam *)malloc(sizeof (audioParam));
    if (handle == NULL) {
        ALOGE("%s malloc is fail ", __func__ );
        return -1;
    }
    memset(handle, 0, sizeof (audioParam));

    handle->numChannels = numChannels;
    handle->format = format;

    ret = sonicWrapper->createSonic((void *)handle, sampleRate, numChannels,speed);
    if (ret < 0 ) {
        ALOGE("%s sonicWrapper->createSonic is fail ", __func__ );
        goto err;
    }

    return 0;
err:
    if (handle) {
        free(handle);
        handle = NULL;
    }
    ALOGE("%s error!! ", __func__ );
    return -1;
}

int jmSonicImp::jmSonicClose()
{
    ALOGD("%s  ", __func__ );
    sonicWrapper->releaseSonic(handle);
    if (handle) {
        free(handle);
        handle = NULL;
    }
    return 0 ;
}

int jmSonicImp::jmSonicProcess(void *inBuffer, size_t *in_size,void *outBuffer,size_t *out_size )
{
    int framesize = 0;
    int minOutsize = 0;
    int numSamples = 0;
    int totalFrame = 0;

    ALOGD("%s  ", __func__ );
    sonicWrapper->writeData2Sonic(handle, inBuffer,*in_size);

    framesize = audio_bytes_per_frame(handle->numChannels,AUDIO_FORMAT_PCM_16_BIT) * handle->numChannels;
    minOutsize = MINUM_SPEED_OUTPUT_FRAMES * framesize;

    do {
        numSamples = sonicWrapper->readData4Sonic(handle, (char *)outBuffer + totalFrame*framesize, minOutsize);
        totalFrame = +numSamples;
    } while (numSamples > 0);
    *out_size = totalFrame * framesize;

    return 0;
}


