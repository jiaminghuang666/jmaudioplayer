//
// Created by jiaming.huang on 2024/6/21.
//

#include  <pthread.h>

#include "ALOG.h"
#include "AudioPlayer.h"
#include "jmaudioplayer.h"
#include "jmaudioplayermsg.h"

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
    ALOGD("%s Url = %s ", __func__, Url);
    if (!Url) {
        ALOGE("%s Url = %s", __func__ ,Url );
        return -1;
    }
    myUrl = Url;
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
        goto err;
    }
    ret = mffmpeg->FFmpegInitResample();
    if (ret) {
        ALOGE("%s FFmpegInitResample fail !!",__func__ );
    }

    return 0;

err:
    if (queue)
        delete(queue);
    if (mffmpeg)
        delete(mffmpeg);
    if (mopenSl)
        delete(mopenSl);
    return -1;
}


void *_decodeAudio(void *args)
{
    ALOGD("%s start ..",__func__ );
    int ret = -1;
    FFmpegWrapper *p = (FFmpegWrapper *)args;

    ret = p->FFmpegDecodeAudio(playing);
    if (ret) {
        ALOGD("%s ret =%d fail !! ..",__func__,ret );
    }
    postEvent(MSG_EOS, 0, 0);
    ALOGD("%s end ..",__func__ );
    pthread_exit(0);

    return NULL;
}

void *_demux(void *args)
{
    ALOGD("%s start ..",__func__ );
    int ret = -1;
    FFmpegWrapper *p = (FFmpegWrapper *)args;
    ret = p->FFmpegDemux(playing);
    if (ret) {
        ALOGD("%s ret =%d fail !! ..",__func__,ret );
    }
    postEvent(MSG_EOS, 1, 0);
    ALOGD("%s end ..",__func__ );

    pthread_exit(0);
    return NULL;
}

int jmAudioPlayer::startDecode()
{
    ALOGD("%s start ..",__func__ );
    pthread_create(&decodeId, NULL, _decodeAudio, mffmpeg);
    return 0;
}

int jmAudioPlayer::startDemux()
{
    ALOGD("%s start ..",__func__ );
    pthread_create(&demuxId, NULL, _demux, mffmpeg);
    return 0;
}

int jmAudioPlayer::start()
{
    ALOGD("%s", __func__ );
    int ret = -1;

    playing  = true;
    ret = startDemux();
    if (ret) {
        ALOGE("%s startDemux Fail !! \n", __func__ );
        return -1;
    }
    ret = startDecode();
    if (ret) {
        ALOGE("%s startDecode Fail !! \n", __func__ );
        return -1;
    }

    audioParam mparam = mffmpeg->getAPara();
    ALOGD("%s numChannels:%d sampleFormat:%d sampleRate:%d !!",__func__ ,mparam.numChannels, mparam.sampleFormat, mparam.sampleRate );
    ret = mopenSl->createOpenSL(&mparam);
    if (ret) {
        ALOGE("%s createOpenSL Fail !! \n", __func__ );
        return -1;
    }

    ret = mopenSl->startRender();
    if (ret) {
        ALOGE("%s startRender Fail !! \n", __func__ );
        return -1;
    }
    return 0;
}

int jmAudioPlayer::stop()
{
    ALOGD("%s", __func__ );
    //pthread_cancel(decodeId);
    //pthread_cancel(demuxId);
    //pthread_join(decodeId, NULL);
    //pthread_join(demuxId, NULL);

    if (mffmpeg)
        mffmpeg->FFmpegRelease();
    if (mopenSl)
        mopenSl->releaseOpenSL();

    if (queue)
        delete(queue);
    if (mffmpeg)
        delete(mffmpeg);
    if (mopenSl)
        delete(mopenSl);
    return 0;
}


int jmAudioPlayer::pause(bool isPause)
{
    ALOGD("%s", __func__ );
    if (!mffmpeg || !mopenSl) {
        ALOGE("%s mffmpeg=%p,mopenSl=%p !! \n", __func__ ,mffmpeg,mopenSl);
        return -1;
    }

    mffmpeg->setPause(isPause);
    mopenSl->setPause(isPause);

    return 0;
}

int jmAudioPlayer::seek()
{
    ALOGD("%s", __func__ );
    return 0;
}

double jmAudioPlayer::getCurrentPosition()
{
    double myPosition;

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











