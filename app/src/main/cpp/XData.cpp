//
// Created by jiaming.huang on 2024/7/2.
//

#include "XData.h"
#include "ALOG.h"
#include "jmUntil.h"

static bool debug = false;

#define MAXFRAME 25

int XData::put(xdata frame)
{
    ALOGD("%s put data into queue ..", __func__ );
    if (frame.size <= 0 || !frame.data) {
        ALOGE("%s put data into queue fail:frame.size = %d  ..", __func__ ,frame.size);
        return -1;
    }

    while (!isExit) {
        frameMutex.lock();
        if (frames.size() >= 100) {
            if (debug) ALOGD("%s, frames queue is full,need wait some time ", __func__ );
            frameMutex.unlock();
            XSleep(1);
            continue;
        }

        frames.push_back(frame);
        break;
    }

    frameMutex.unlock();
    return 1;
}

xdata XData::get()
{
    ALOGD("%s get data into queue ..", __func__ );
    xdata d;

    while(!isExit) {
        frameMutex.lock();
        if (!frames.empty()) {
            d = frames.front();
            frames.pop_front();
            frameMutex.unlock();
            return d;
        }
        frameMutex.unlock();
        XSleep(1);
    }
    return d;
}


int XData::blockPut(xdata frame)
{
    if (debug) ALOGD("%s  data into queue ..", __func__ );
    //pthread_mutex_lock(&mutex);
    if (frames.size() >= MAXFRAME ) {
        if (debug) ALOGD("%s, frames.size() = %d frames queue is full,need wait some time ", __func__, frames.size() );
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
            if (frames.size() < MAXFRAME ) {
                //if (debug) ALOGD("%s, frames.size() = %d is not full sent full_signal", __func__, frames.size() );
                pthread_cond_signal(&full_signal);
            }
            if (debug) ALOGD("%s, current frames.size() = %d  ", __func__, frames.size() );
            //pthread_mutex_unlock(&mutex);
            return  d;
        }
    }
    if (debug) ALOGD("%s, do nothing frames.size() = %d  ", __func__, frames.size() );
   // pthread_mutex_unlock(&mutex);
    return d;
}


