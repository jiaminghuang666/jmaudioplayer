//
// Created by jiaming.huang on 2024/8/6.
//

#ifndef JMAUDIOPLAYER_FFMPEGQUEUE_H
#define JMAUDIOPLAYER_FFMPEGQUEUE_H

#include "ALOG.h"
#include "jmUntil.h"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
};

#define  QUEUE_MAX 25

static bool queuedebug = false;

typedef struct Node_t {
    void * data;
    long int pktindex;
    struct Node_t *next;
} Node;

typedef struct Queue_t {
    Node *head;
    Node *tail;
    pthread_cond_t EmtySignal;
    pthread_cond_t FullSignal;
    pthread_mutex_t mutex;
    int size;
}Queue;

inline static void initQueue(Queue *pktQueue)
{
    ALOGE("%s",__func__ );
    pktQueue->head = NULL;
    pktQueue->tail = NULL;

    pthread_mutex_init(&pktQueue->mutex, NULL);
    pthread_cond_init(&pktQueue->EmtySignal, NULL);
    pthread_cond_init(&pktQueue->FullSignal, NULL);

    pktQueue->size = 0;
    ALOGE("%s end",__func__ );
}

inline static void destroyQueue(Queue *pktQueue)
{
    ALOGE("%s",__func__ );
    Node *tempNode;

    while (pktQueue->head != NULL) {
        tempNode = pktQueue->head;
        pktQueue->head = pktQueue->head->next;
        free(tempNode);
    }
    pthread_mutex_destroy(&pktQueue->mutex);
    pthread_cond_destroy(&pktQueue->EmtySignal);
    pthread_cond_destroy(&pktQueue->FullSignal);
    ALOGE("%s end",__func__ );

    return ;
}

inline Node * CreateNode(void *data,int pktindex )
{

    Node *newNode = (Node *) malloc(sizeof (Node));
    if (newNode == NULL) {
        ALOGE("%s Create node is fail !! \n",__func__ );
        return NULL;
    }
    memset(newNode, 0, sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    newNode->pktindex = pktindex;

    return newNode;
}

inline static void enqueue(Queue *pktQueue, void * data,int pktindex)
{
    Node *newNode = CreateNode(data,pktindex );

    if (queuedebug) ALOGD("%s pktindex=%d ...",__func__ ,pktindex );

    pthread_mutex_lock(&pktQueue->mutex);
    if (pktQueue->size == QUEUE_MAX) {
        if (queuedebug)
            ALOGD("%s enqueue is full should wait for FullSignal,size =%d ...",__func__ ,pktQueue->size);
        pthread_cond_wait(&pktQueue->FullSignal, &pktQueue->mutex);
    }

    if (pktQueue->tail == NULL) {
        ALOGD("%s create first node ...",__func__ );
        pktQueue->head = newNode;
    } else {
        pktQueue->tail->next = newNode;
    }

    pktQueue->tail = newNode;
    pktQueue->size++;

    pthread_cond_signal(&pktQueue->EmtySignal);
    pthread_mutex_unlock(&pktQueue->mutex);

    if (queuedebug) ALOGD("%s end pktindex =%d...",__func__ ,pktindex);
    return;
}

inline static void * dequeue(Queue *pktQueue, long int * pktindex)
{
    if (queuedebug) ALOGD("%s",__func__ );
    void *data = NULL;

    pthread_mutex_lock(&pktQueue->mutex);
    while (pktQueue->size == 0 ) {
        if (queuedebug)
            ALOGD("%s queue is Emty wait for EmtySignal ...",__func__ );
        pthread_cond_wait(&pktQueue->EmtySignal, &pktQueue->mutex);
    }

    Node *tempNode = pktQueue->head;
    data = tempNode->data;
    *(pktindex) = tempNode->pktindex;
    if (queuedebug) ALOGD("%s pktindex=%d ",__func__, tempNode->pktindex);
    pktQueue->head = tempNode->next;

    if (pktQueue->head == NULL) {
        pktQueue->tail = NULL;
        ALOGD("%s head == NULL ...",__func__ );
    }

    pktQueue->size--;
    free(tempNode);
    if ( pktQueue->size < QUEUE_MAX) {
        pthread_cond_signal(&pktQueue->FullSignal);
    }
    pthread_mutex_unlock(&pktQueue->mutex);

    if (queuedebug) ALOGD("%s  end",__func__);
    return data;
}

#endif //JMAUDIOPLAYER_FFMPEGQUEUE_H
