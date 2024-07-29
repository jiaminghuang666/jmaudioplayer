#include <jni.h>

// Write C++ code here.
//
// Do not forget to dynamically load the C++ library into your application.
//
// For instance,
//
// In MainActivity.java:
//    static {
//       System.loadLibrary("jmaudioplayer");
//    }
//
// Or, in MainActivity.kt:
//    companion object {
//      init {
//         System.loadLibrary("jmaudioplayer")
//      }
//    }

#include "ALOG.h"
#include "AudioPlayer.h"

jmAudioPlayer * mjmAudioPlayer = jmAudioPlayer::getInstant();


extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jmaudioplayer_PlaybackActivity_setDataSource(JNIEnv *env, jobject thiz,
                                                              jstring url, jobject handle) {
    // TODO: implement setDataSource()
    ALOGD("%s start ",__func__);
    int ret = -1;
    //mjmAudioPlayer = jmAudioPlayer::getInstant();
    const char *myurl = env->GetStringUTFChars(url, 0);
    ret = mjmAudioPlayer->createjmAudioPlayer(myurl);
    env->ReleaseStringUTFChars(url,myurl );

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jmaudioplayer_PlaybackActivity_releaseSource(JNIEnv *env, jobject thiz) {
    // TODO: implement releaseSource()
    mjmAudioPlayer->releasejmAudioPlayer();
    if(mjmAudioPlayer != nullptr)
        delete mjmAudioPlayer;
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_jmaudioplayer_PlaybackActivity_startPlay(JNIEnv *env, jobject thiz) {
    // TODO: implement startPlay()
    mjmAudioPlayer->play();
    return;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_jmaudioplayer_PlaybackActivity_stopPlay(JNIEnv *env, jobject thiz) {
    // TODO: implement stopPlay()
    return;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_jmaudioplayer_PlaybackActivity_pause(JNIEnv *env, jobject thiz) {
    // TODO: implement pause()
    return;
}
