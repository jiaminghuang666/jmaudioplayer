//
// Created by jiaming.huang on 2024/6/19.
//

#include "jmSonicImp.h"
#include "ALOG.h"
#include "jmUntil.h"

extern "C" {
#include <cstdlib>
#include <cstring>
#include <jni.h>

};

#define MINUM_SPEED_OUTPUT_FRAMES 512

sonic_wrapper::sonic_wrapper()
{
    ALOGD("sonic_wrapper::sonic_wrapper()");
}

sonic_wrapper::~sonic_wrapper()
{
    ALOGD("sonic_wrapper::~sonic_wrapper");
}

int sonic_wrapper::createSonic(void *mhandle,int sampleRate,int numChannels,float speed)
{
    float pitch = 1.0f;
    float rate = 1.0f;
    float volume = 1.0f;
    int emulateChordPitch = 0;
    int quality = 0;

    ALOGD("%s",__func__);
    audioParam * handle = (audioParam *)mhandle;

    handle->stream = sonicCreateStream(sampleRate,numChannels);
    if (handle->stream == NULL) {
        ALOGE("%s sonicCreateStream create fail",__func__);
        return -1;
    }

    sonicSetSpeed(handle->stream, speed);
    sonicSetPitch(handle->stream, pitch);
    sonicSetRate(handle->stream, rate);
    sonicSetVolume(handle->stream, volume);
    sonicSetChordPitch(handle->stream, emulateChordPitch);
    sonicSetQuality(handle->stream, quality);

    handle->numChannels = numChannels;

    return 0;
}

int sonic_wrapper::writeData2Sonic(void *mhandle, void *inBuffer, size_t in_size)
{
    int ret = -1;
    int in_frame = 0;
    audioParam * handle = (audioParam *)mhandle;

    ALOGD("%s",__func__);
    in_frame = in_size / audio_bytes_per_frame(handle->numChannels,(audio_format_t)handle->format);
    ALOGD("%s  in_frame = %d ",__func__, in_frame);
    ret = sonicWriteShortToStream(handle->stream, (short  *)inBuffer, in_frame);
    if (ret == 0) {
        ALOGE("%s sonicWriteShortToStream input fail ",__func__);
    }

    return in_frame;
}

int sonic_wrapper::readData4Sonic(void *mhandle, void *outBuffer,size_t out_size)
{
    int out_frame = 0;
    int samplesWritten = 0;
    audioParam * handle = (audioParam *)mhandle;

    ALOGD("%s",__func__);
    out_frame = out_size / audio_bytes_per_frame(handle->numChannels,(audio_format_t)handle->format);
    samplesWritten = sonicReadShortFromStream(handle->stream, (short  *)outBuffer,out_frame);
    ALOGD("%s  in_frame = %d samplesWritten=%d ",__func__, out_frame, samplesWritten);

    return samplesWritten;
}

int sonic_wrapper::releaseSonic(void *mhandle)
{
    ALOGD("%s",__func__);
    audioParam * handle = (audioParam *)mhandle;
    if (handle->stream) {
        sonicDestroyStream(handle->stream);
    }

    return 0;
}



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


