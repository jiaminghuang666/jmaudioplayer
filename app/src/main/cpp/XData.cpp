//
// Created by jiaming.huang on 2024/7/2.
//

#include "XData.h"
#include "ALOG.h"
#include "jmUntil.h"

static bool debug = true;

#define MAXFRAME 25

int XData::blockPut(xdata frame)
{
    if (debug) ALOGD("%s  start ..", __func__ );
    if (frame.size <= 0 || !frame.data) {
        ALOGE("%s, xdata frame.size = %d  null fail !! ", __func__, frame.size );
        return -1;
    }

    //pthread_mutex_lock(&mutex);
    if (frames.size() > MAXFRAME ) {
        ALOGD("%s, frames.size() = %d frames queue full frames.empty() ", __func__, frames.size() );
        pthread_cond_wait(&full_signal, &mutex);
    }
    if (debug) ALOGD("%s, current frames.size() = %d  frameindex =%ld", __func__, frames.size(), frame.frameindex );
    frames.push_back(frame);
   // pthread_mutex_unlock(&mutex);
    if (debug) ALOGD("%s  end", __func__ );
    return 1;
}

xdata XData::blockGet()
{
    if (debug) ALOGD("%s  start..", __func__ );
    xdata d;
    //pthread_mutex_lock(&mutex);
    //while(!isExit) {
        if(!frames.empty()) {
            if (frames.size() < MAXFRAME ) {
                ALOGD("%s, frames.size() = %d sent full_signal ", __func__, frames.size() );
                pthread_cond_signal(&full_signal);
            }

            d = frames.front();
            if (debug) ALOGD("%s, current frames.size() = %d  frameindex =%ld \n", __func__, frames.size(), d.frameindex );
            frames.pop_front();
            //pthread_mutex_unlock(&mutex);
            return  d;
        } else {
            if (debug) ALOGD("%s, frames.empty() = %d   ", __func__, frames.empty() );
        }
    //}
   // pthread_mutex_unlock(&mutex);

    if (debug) ALOGD("%s  end..", __func__ );
    return d;
}


