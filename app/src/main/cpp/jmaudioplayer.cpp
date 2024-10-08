#include <jni.h>
#include <android/native_window_jni.h>
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
#include "jmaudioplayer.h"

jmAudioPlayer * mjmAudioPlayer = jmAudioPlayer::getInstant();

#define JNI_CLASS_NAME  "com/example/jmaudioplayer/JMAudioPlayer"
JavaVM* gJavaVM = NULL;
jclass g_myjclass;
jobject gJavaObject;


extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *res) {
    ALOGD("%s  start ",__func__);
    JNIEnv * env;
    if ((*vm).GetEnv( (void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGD("%s  getenv fail ",__func__);
        return -1;
    }
    gJavaVM = vm;

    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_testFan(JNIEnv *env, jobject thiz) {
    // TODO: implement testFan()

    gJavaObject = env->NewGlobalRef(thiz);

    return;
}

void jniPostEvent_n(int id, int arg1,int arg2)
{
    ALOGD("%s  start",__func__);
    JNIEnv * env;
    if (gJavaVM->AttachCurrentThread(&env, NULL) != JNI_OK) {
        return ;
    }

    //jstring classPath = env->NewStringUTF(JNI_CLASS_NAME);
    jclass myjclass = env->FindClass(JNI_CLASS_NAME);
    if (myjclass == NULL) {
        if (env->ExceptionOccurred()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        gJavaVM->DetachCurrentThread();
        ALOGD("jiaming jni post_event jclazz fail  \n");
        return ;
    }

    g_myjclass = reinterpret_cast<jclass>(env->NewGlobalRef(myjclass));
    env->DeleteLocalRef(myjclass); // 删除局部引用

    jmethodID jmID = env->GetMethodID(g_myjclass, "postEventFromNative", "(III)V");
    if (jmID == NULL || env->ExceptionOccurred()) {
        ALOGE("%s  env->GetMethodID fail ",__func__);
        // GetMethodID失败或发生异常处理
        if (env->ExceptionOccurred()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        // 清理
        env->DeleteGlobalRef(g_myjclass);
        gJavaVM->DetachCurrentThread();
        return ;
    }

    env->CallVoidMethod(gJavaObject, jmID, id, arg1, arg2);

    // env->DeleteLocalRef(classPath);
    ALOGD("%s end",__func__);

    return;
}

void jniPostEvent(int id, int arg1,int arg2)
{
    jniPostEvent_n(id, arg1, arg2);
    return ;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_getParam(JNIEnv *env, jobject thiz, jint id) {
    // TODO: implement getParam()
    jlong value;
    mjmAudioPlayer->getParam(id, &value);

    return value;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_start(JNIEnv *env, jobject thiz) {
    // TODO: implement start()
    ALOGD("%s start ",__func__);
    mjmAudioPlayer->start();

    return;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_setDataSource(JNIEnv *env, jobject thiz, jstring url,
                                                           jobject handle) {
    // TODO: implement setDataSource()
    ALOGD("%s start ",__func__);
    int ret = -1;

    const char *myurl = env->GetStringUTFChars(url, 0);
    ret = mjmAudioPlayer->setdataSource(myurl);
    if (ret != 0 ){
        return ret;
    }

    ret = mjmAudioPlayer->prepareAsync();

    env->ReleaseStringUTFChars(url,myurl );

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_prepareAsync(JNIEnv *env, jobject thiz) {
    // TODO: implement prepareAsync()
    ALOGD("%s start ",__func__);
    int ret = -1;
   // ret = mjmAudioPlayer->prepareAsync();

    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_releaseSource(JNIEnv *env, jobject thiz) {
    // TODO: implement releaseSource()
    ALOGD("%s start ",__func__);
    env->DeleteGlobalRef (g_myjclass);
    g_myjclass = NULL;
    ALOGD("%s end 1",__func__);
    env->DeleteGlobalRef(gJavaObject);
    gJavaObject = NULL; // 将指针设置为NULL，避免野指针
    gJavaVM->DetachCurrentThread();

    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_stop(JNIEnv *env, jobject thiz) {
    // TODO: implement stop()
    mjmAudioPlayer->stop();
    if(mjmAudioPlayer != nullptr)
        delete mjmAudioPlayer;
    return;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_pause(JNIEnv *env, jobject thiz, jboolean is_pause) {
    // TODO: implement pause()
    int ret = 0;
    ret = mjmAudioPlayer->pause(is_pause);
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_seek(JNIEnv *env, jobject thiz) {
    // TODO: implement seek()
    mjmAudioPlayer->seek();
    return;
}

extern "C"
JNIEXPORT jdouble  JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_getCurrentPosition(JNIEnv *env, jobject thiz) {
    // TODO: implement getCurrentPosition()
    return mjmAudioPlayer->getCurrentPosition();
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_jmaudioplayer_JMAudioPlayer_getDuration(JNIEnv *env, jobject thiz) {
    // TODO: implement getDuration()
    return mjmAudioPlayer->getDuration();
}





