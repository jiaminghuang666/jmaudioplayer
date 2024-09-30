//
// Created by jiaming.huang on 2024/6/25.
//

#include "openSLWrapper.h"
#include "ALOG.h"
#include "jmUntil.h"

static bool opensldebug = true;

static SLObjectItf engineSL = NULL;
static SLEngineItf eng = NULL;
static SLObjectItf mix = NULL;
static SLObjectItf player = NULL;
static SLPlayItf iplayer = NULL;
static SLAndroidSimpleBufferQueueItf pcmQue = NULL;

openSLWrapper::openSLWrapper(XData * queue)
{
    ALOGD("%s",__func__ );
    mqueue = queue;
    buf = new unsigned char[1024*1024];
    memset(buf, 0, sizeof(unsigned char)*1024*1024 );
}

openSLWrapper::~openSLWrapper()
{
    ALOGD("%s",__func__ );
    delete buf;
    buf = 0;
}

static SLEngineItf CreateSL()
{
    SLresult re;
    SLEngineItf en;
    re = slCreateEngine(&engineSL,0,0,0,0,0);
    if (re != SL_RESULT_SUCCESS) return NULL;
    re = (*engineSL)->Realize(engineSL,SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) return NULL;
    re = (*engineSL)->GetInterface(engineSL,SL_IID_ENGINE, &en);
    if (re != SL_RESULT_SUCCESS) return NULL;
    return en;
}

static int dumprenderData(void *data, int size)
{
    FILE *file = fopen("/data/outputrender.pcm","a");
    if (!file) {
        ALOGE("%s get data is not enough size:%d  ..", __func__ ,size);
        return -1;
    } else {
       fwrite(data, 1, size, file);
       fclose(file);
    }
    return 0;
}

void openSLWrapper::PlayCall(void *bufq)
{
    if (!bufq) return;
    SLAndroidSimpleBufferQueueItf bf = (SLAndroidSimpleBufferQueueItf)bufq;

    xdata mxdata = mqueue->blockGet();
    if (mxdata.size <= 0) {
	   ALOGE("%s get data is not enough frame size:%d  ..", __func__ ,mxdata.size);
	   return ;
	}

    //if (opensldebug) {
        ALOGD("%s  blockGet render frameindex = %ld : frame size = %d", __func__ ,mxdata.frameindex, mxdata.size);
        dumprenderData(mxdata.data,mxdata.size);
    //}
    memcpy(buf, mxdata.data, mxdata.size);
	if (pcmQue && (*pcmQue)) {
        (*pcmQue)->Enqueue(pcmQue,buf, mxdata.size);
	}
    mqueue->DropFrameBuffer(mxdata.data);

    //return ;
}

static void PcmCall(SLAndroidSimpleBufferQueueItf bf, void *context)
{
    openSLWrapper *ap = (openSLWrapper *)context;
    if (!ap) {
        ALOGE("%s CreateSL fail !!", __func__ );
        return ;
    }
    ap->PlayCall((void *)bf);

    return;
}

int openSLWrapper::createOpenSL(void *Param)
{
    int ret = -1;
    audioParam *outParam = (audioParam *)Param;

    eng = CreateSL(); //1 创建引擎
    if ( eng ) {
        ALOGD("%s CreateSL successful ..", __func__ );
    } else {
        ALOGE("%s CreateSL fail !!", __func__ );
        return -1;
    }

    SLresult re = 0 ; // 2 创建混音器
    re = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS ) {
        ALOGE("%s CreateOutputMix fail !!", __func__ );
        return -1;
    }

    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        ALOGE("%s (*mix)->Realize fail !!", __func__ );
        return  -1;
    }

    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outmix, 0};

    //3 配置音频信息 缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32) outParam->numChannels,
            (SLuint32) outParam->sampleRate*1000,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSource ds = {&que, &pcm};

    //4 创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink, sizeof (ids) / sizeof (SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        ALOGE("%s CreateAudioPlayer fail !!", __func__ );
        return  -1;
    }
    (*player)->Realize(player, SL_BOOLEAN_FALSE);

    //获取player接口
    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if (re != SL_RESULT_SUCCESS) {
        ALOGE("%s GetInterface SL_IID_PLAY fail !!", __func__ );
        return  -1;
    }
    re = (*player)->GetInterface(player,SL_IID_BUFFERQUEUE, &pcmQue);
    if (re != SL_RESULT_SUCCESS) {
        ALOGE("%s GetInterface SL_IID_BUFFERQUEUE fail !!", __func__ );
        return  -1;
    }

    //设置回调函数，播放队列空调用
    (*pcmQue)->RegisterCallback(pcmQue,PcmCall,this);

    //设置为播放状态
    (*iplayer)->SetPlayState(iplayer,SL_PLAYSTATE_PAUSED);

    //启动队列回调
    (*pcmQue)->Enqueue(pcmQue,"",1);

    ALOGD("%s successful ..", __func__ );
    return 0 ;
}


int openSLWrapper::releaseOpenSL()
{
    ALOGD("%s  start ..", __func__ );
    if (*iplayer)
        (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_STOPPED);

    if (pcmQue && (*pcmQue))
        (*pcmQue)->Clear(pcmQue);

    if(player && (*player))
        (*player)->Destroy(player);

    if(mix && (*mix))
        (*mix)->Destroy(mix);

    if(engineSL && (*engineSL))
        (*engineSL)->Destroy(engineSL);

    engineSL = NULL;
    mix = NULL;
    player = NULL;
    pcmQue = NULL;
    iplayer = NULL;

    return 0;
}

void openSLWrapper::renderpcm()
{
    PcmCall(pcmQue,this);
    return ;
}

void * _renderpcm(void *args)
{
    ALOGD("%s start ..", __func__ );
    openSLWrapper *p = (openSLWrapper *)args;
    //p->renderpcm();

    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);
    pthread_exit(0);
    ALOGD("%s end ..", __func__ );
    return NULL;
}

int openSLWrapper::startRender()
{
    ALOGD("%s start ..", __func__ );
    pthread_create(&renderId, NULL, _renderpcm, this);
    return 0;
}

int  openSLWrapper::setPause(bool isPause)
{
    isPauseing = isPause;
    if (isPauseing) {
        ALOGD("%s SL_PLAYSTATE_PLAYING ..", __func__ );
        if (*iplayer)
            (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PAUSED);
    } else {
        ALOGD("%s SL_PLAYSTATE_PLAYING ..", __func__ );
        if (*iplayer)
            (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);
    }

    return 0;
}