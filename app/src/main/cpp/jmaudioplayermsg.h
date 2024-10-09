//
// Created by jiaming.huang on 2024/10/9.
//

#ifndef JMAUDIOPLAYER_JMAUDIOPLAYERMSG_H
#define JMAUDIOPLAYER_JMAUDIOPLAYERMSG_H

#include "jmaudioplayer.h"
#include "ALOG.h"

enum MSG {
    MSG_START = 0,
    MSG_ERROR = 100,
    MSG_INFO = 200,
    MSG_EOS   = 300,
};

static inline int postEvent(int id,int arg1,int arg2)
{
    switch (id) {
        case  MSG_START:
            ALOGD("%s = MSG_START  !!",__func__ );
            jniPostEvent(MSG_START, arg1, arg2);
            break;
        case MSG_ERROR:
            ALOGD("%s = MSG_ERROR  !!",__func__ );
            jniPostEvent(MSG_ERROR, arg1, arg2);
            break;
        case MSG_INFO:
            ALOGD("%s = MSG_INFO  !!",__func__ );
            jniPostEvent(MSG_INFO, arg1, arg2);
            break;
        case MSG_EOS:
            ALOGD("%s = MSG_EOS  !!",__func__ );
            jniPostEvent(MSG_EOS, arg1, arg2);
            break;
        default:
            ALOGD("%s = default  !!",__func__ );
            break;
    }
    return 0;
}

#endif //JMAUDIOPLAYER_JMAUDIOPLAYERMSG_H
