//
// Created by jiaming.huang on 2024/6/21.
//

#include "ALOG.h"
#include "AudioPlayer.h"

#include "FFmpegWrapper.h"
#include "openSLWrapper.h"

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

    audioParam mparam = mffmpeg->getAPara();
    ALOGD("%s numChannels:%d sampleFormat:%d sampleRate:%d !!",__func__ ,mparam.numChannels, mparam.sampleFormat, mparam.sampleRate );
    ret = mopenSl->createOpenSL(&mparam);

    ret = mffmpeg->startDecode(true);

    ret = mopenSl->startRender();

    return 0;
}


