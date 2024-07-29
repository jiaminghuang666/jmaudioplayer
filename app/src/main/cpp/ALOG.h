//
// Created by jiaming.huang on 2024/6/18.
//

#ifndef JMAUDIOSONIC_ALOG_H
#define JMAUDIOSONIC_ALOG_H

#ifdef ANDROID
#include <android/log.h>
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"jiaming",__VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO,"jiaming",__VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR,"jiaming",__VA_ARGS__)
#else
#define ALOGD(...) printf("jiaming",__VA_ARGS__)
#define ALOGI(...) printf("jiaming",__VA_ARGS__)
#define ALOGE(...) printf("jiaming",__VA_ARGS__)
#endif

#endif //JMAUDIOSONIC_ALOG_H
