//
// Created by jiaming.huang on 2024/6/21.
//

#include "ALOG.h"
#include "AudioPlayer.h"

#include "FFmpegWrapper.h"
#include "openSLWrapper.h"

#include "jmaudioplayer.h"

jmAudioPlayer::jmAudioPlayer()
{
    ALOGD("%s", __func__ );

}

jmAudioPlayer::~jmAudioPlayer()
{
    ALOGD("%s", __func__ );
}

int jmAudioPlayer::createjmAudioPlayer(const char *Url)
{
    int ret = -1;

    queue = new XData();
    mffmpeg = new FFmpegWrapper(queue);
    mopenSl = new openSLWrapper(queue);

    ret = mffmpeg->FFmpegInit(Url);
    if (ret) {
        ALOGE("%s FFmpegInit fail !!",__func__ );
    }
    ret = mffmpeg->FFmpegInitResample();
    if (ret) {
        ALOGE("%s FFmpegInitResample fail !!",__func__ );
    }

    return 0;
}

int jmAudioPlayer::releasejmAudioPlayer()
{
    mffmpeg->FFmpegRelease();
    mopenSl->releaseOpenSL();

    delete(queue);
    return 0;
}

int jmAudioPlayer::play()
{
    int ret = -1;

    ret = mffmpeg->startDemux(true);

    audioParam mparam = mffmpeg->getAPara();
    ALOGD("%s numChannels:%d sampleFormat:%d sampleRate:%d !!",__func__ ,mparam.numChannels, mparam.sampleFormat, mparam.sampleRate );
    ret = mopenSl->createOpenSL(&mparam);

    ret = mffmpeg->startDecode(true);

    ret = mopenSl->startRender();

    return 0;
}

int jmAudioPlayer::getParam(int id, void *param)
{
    if (!param)
        return -1;

    switch (id) {
        case PARAM_DURATION:
            *(int64_t *)param = mffmpeg->getDuration();
            ALOGD("%s duration %lld  !!",__func__ ,*(int64_t *)param );
            break;
        case PARAM_POSITION:
            break;

        default:
            ALOGE("%s  fail !!",__func__ );
            break;
    }

    return 0;
}

int jmAudioPlayer::postEvent(int id,int arg1,int arg2)
{

    jniPostEvent(id, arg1, arg2);

    return 0;
}

