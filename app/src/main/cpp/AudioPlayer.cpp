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

int jmAudioPlayer::setdataSource(const char *Url)
{
    ALOGD("%s", __func__ );
    if (!Url) {
        ALOGE("%s Url = %s", __func__ ,Url );
        return -1;
    }
    myUrl = Url;
    ALOGE("%s Url = %d", __func__ ,Url );
    return 0;
}


int jmAudioPlayer::prepareAsync()
{
    ALOGD("%s", __func__ );
    int ret = -1;

    queue = new XData();
    mffmpeg = new FFmpegWrapper(queue);
    mopenSl = new openSLWrapper(queue);

    ret = mffmpeg->FFmpegInit(myUrl);
    if (ret) {
        ALOGE("%s FFmpegInit fail !!",__func__ );
    }
    ret = mffmpeg->FFmpegInitResample();
    if (ret) {
        ALOGE("%s FFmpegInitResample fail !!",__func__ );
    }

    return 0;
}


int jmAudioPlayer::start()
{
    ALOGD("%s", __func__ );
    int ret = -1;

    ret = mffmpeg->startDemux(true);
    ret = mffmpeg->startDecode(true);

    audioParam mparam = mffmpeg->getAPara();
    ALOGD("%s numChannels:%d sampleFormat:%d sampleRate:%d !!",__func__ ,mparam.numChannels, mparam.sampleFormat, mparam.sampleRate );
    ret = mopenSl->createOpenSL(&mparam);

    ret = mopenSl->startRender();
    return 0;
}

int jmAudioPlayer::stop()
{
    ALOGD("%s", __func__ );
    mffmpeg->FFmpegRelease();
    mopenSl->releaseOpenSL();

    delete(queue);
    return 0;
}

int jmAudioPlayer::pause()
{
    ALOGD("%s", __func__ );
    return 0;
}

int jmAudioPlayer::seek()
{
    ALOGD("%s", __func__ );
    return 0;
}

long int jmAudioPlayer::getCurrentPosition()
{
    int64_t myPosition;

    myPosition = mffmpeg->getCurrentPosition();
    //ALOGD("%s myPosition= %lld  !!",__func__ ,myPosition );

    return myPosition;
}

long int jmAudioPlayer::getDuration()
{
    int64_t myDuration;

    myDuration = mffmpeg->getDuration();
    //ALOGD("%s duration= %lld  !!",__func__ , myDuration );

   return myDuration;
}


int jmAudioPlayer::getParam(int id, void *param)
{
    if (!param)
        return -1;

    switch (id) {
        /*case PARAM_DURATION:
            *(int64_t *)param = mffmpeg->getDuration();
            ALOGD("%s duration %lld  !!",__func__ ,*(int64_t *)param );
            break;
        case PARAM_POSITION:
            break;*/

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









