//
// Created by jiaming.huang on 2024/7/2.
//

#ifndef JMAUDIOSONIC_XDATA_H
#define JMAUDIOSONIC_XDATA_H

#include <list>
#include <mutex>
#include <thread>

typedef struct  audioParam_t {
    int numChannels;
    int sampleRate;
    int sampleFormat;
}audioParam ;

typedef struct xdata_t {
    uint8_t * data = 0;
    int size = 0;
    long int frameindex = 0;
} xdata;


class XData {
public:

    virtual uint8_t * AllocFrameBuffer(int size);
    virtual int DropFrameBuffer(uint8_t *data);
    virtual int blockPut(xdata frame);
    virtual xdata blockGet();

    bool isExit = false;

private:
    std::list <xdata> frames;
    std::mutex frameMutex;

    pthread_mutex_t mutex;
    pthread_cond_t full_signal;


};



#endif //JMAUDIOSONIC_XDATA_H
