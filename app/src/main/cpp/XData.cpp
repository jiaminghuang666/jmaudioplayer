//
// Created by jiaming.huang on 2024/7/2.
//

#include "XData.h"
#include "ALOG.h"
#include "jmUntil.h"

static bool debug = false;

#define MAXFRAME 25

int XData::blockPut(xdata frame)
{
    if (debug) ALOGD("%s  data into queue ..", __func__ );
    //pthread_mutex_lock(&mutex);
    if (frames.size() >= MAXFRAME ) {
       // if (debug)
            ALOGD("%s, frames.size() = %d frames queue is full,need wait full_signal ", __func__, frames.size() );
        pthread_cond_wait(&full_signal, &mutex);
    }
    if (debug) ALOGD("%s, current frames.size() = %d  ", __func__, frames.size() );
    frames.push_back(frame);
   // pthread_mutex_unlock(&mutex);
    return 1;
}


xdata XData::blockGet()
{
    if (debug) ALOGD("%s  data from queue ..", __func__ );
    xdata d;
    //pthread_mutex_lock(&mutex);
    while(!isExit) {
        if(!frames.empty()) {
            d = frames.front();
            frames.pop_front();
            //if (frames.size() < MAXFRAME ) {
                //if (debug)
                ALOGD("%s, frames.size() = %d is not full sent full_signal", __func__, frames.size() );
                pthread_cond_signal(&full_signal);
            //}
            if (debug) ALOGD("%s, current frames.size() = %d  ", __func__, frames.size() );
            //pthread_mutex_unlock(&mutex);
            return  d;
        }
    }
    if (debug) ALOGD("%s, do nothing frames.size() = %d  ", __func__, frames.size() );
   // pthread_mutex_unlock(&mutex);
    return d;
}


