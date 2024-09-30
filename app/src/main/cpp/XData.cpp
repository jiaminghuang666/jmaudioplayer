//
// Created by jiaming.huang on 2024/7/2.
//

#include "XData.h"
#include "ALOG.h"
#include "jmUntil.h"

static bool debug = true;

#define MAXFRAME 25


uint8_t * XData::AllocFrameBuffer(int size)
{
    uint8_t * data = (uint8_t *) malloc(size);
    if (!data) {
        ALOGE("%s, mxdata malloc fail !! ", __func__ );
        return NULL;
    }

    return data;
}

int XData::DropFrameBuffer(uint8_t *data)
{
    free(data);
    return 0;
}

int XData::blockPut(xdata mxdata)
{
    if (debug) ALOGD("%s  start ..", __func__ );
    if (mxdata.size <= 0 || !mxdata.data) {
        ALOGE("%s, xdata frame.size = %d  null fail !! ", __func__, mxdata.size );
        return -1;
    }

    //pthread_mutex_lock(&mutex);
    if (frames.size() > MAXFRAME ) {
        ALOGD("%s, frames.size() = %d frames queue full frames.empty() ", __func__, frames.size() );
        pthread_cond_wait(&full_signal, &mutex);
    }
    if (debug) ALOGD("%s, current frames.size() = %d  frameindex =%ld", __func__, frames.size(), mxdata.frameindex );
    frames.push_back(mxdata);
   // pthread_mutex_unlock(&mutex);
    if (debug) ALOGD("%s  end", __func__ );
    return 1;
}

xdata XData::blockGet()
{
    if (debug) ALOGD("%s  start..", __func__ );
    xdata mxdata;
    //pthread_mutex_lock(&mutex);
    //while(!isExit) {
        if(!frames.empty()) {
            if (frames.size() < MAXFRAME ) {
                ALOGD("%s, frames.size() = %d sent full_signal ", __func__, frames.size() );
                pthread_cond_signal(&full_signal);
            }

            mxdata = frames.front();
            if (debug) ALOGD("%s, current frames.size() = %d  frameindex =%ld \n", __func__, frames.size(), mxdata.frameindex );
            frames.pop_front();
            //pthread_mutex_unlock(&mutex);
            return  mxdata;
        } else {
            if (debug) ALOGD("%s, frames.empty() = %d   ", __func__, frames.empty() );
        }
    //}
   // pthread_mutex_unlock(&mutex);

    if (debug) ALOGD("%s  end..", __func__ );
    return mxdata;
}


