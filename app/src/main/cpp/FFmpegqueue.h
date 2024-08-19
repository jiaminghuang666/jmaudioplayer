//
// Created by jiaming.huang on 2024/8/6.
//

#ifndef JMAUDIOPLAYER_FFMPEGQUEUE_H
#define JMAUDIOPLAYER_FFMPEGQUEUE_H

#include "ALOG.h"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
};

#define  QUEUE_MAX 25
static bool debug = true;

typedef struct Node_t {
    void * data;
    struct Node_t *next;
} Node;

typedef struct Queue_t {
    Node *front;
    Node *rear;
    pthread_cond_t notEmtySignal;
    pthread_cond_t FullSignal;
    pthread_mutex_t mutex;
    int size;
}Queue;

inline static void initQueue(Queue *pktQueue)
{
    ALOGE("%s",__func__ );
    pktQueue->front = NULL;
    pktQueue->rear = NULL;

    pthread_mutex_init(&pktQueue->mutex, NULL);
    pthread_cond_init(&pktQueue->notEmtySignal, NULL);
    pthread_cond_init(&pktQueue->FullSignal, NULL);

    pktQueue->size = 0;
    ALOGE("%s end",__func__ );
}

inline static void destroyQueue(Queue *pktQueue)
{
    ALOGE("%s",__func__ );
    Node *tempNode;

    while (pktQueue->front != NULL) {
        tempNode = pktQueue->front;
        pktQueue->front = pktQueue->front->next;
        free(tempNode);
    }
    pthread_mutex_destroy(&pktQueue->mutex);
    pthread_cond_destroy(&pktQueue->notEmtySignal);
    ALOGE("%s end",__func__ );

    return ;
}

inline static void enQueue(Queue *pktQueue, void * data)
{
    Node *newNode = (Node *) malloc(sizeof (Node));
    newNode->data = data;
    newNode->next = NULL;

    //ALOGE("%s ...",__func__ );

    pthread_mutex_lock(&pktQueue->mutex);
    if (pktQueue->size == QUEUE_MAX) {
        pthread_cond_wait(&pktQueue->FullSignal, &pktQueue->mutex);
        if (debug)
            ALOGE("%s enqueue is full should wait for FullSignal,size =%d ...",__func__ ,pktQueue->size);
    }

    if (pktQueue->rear == NULL) {
        pktQueue->front = newNode;
    } else {
        pktQueue->rear->next = newNode;
    }

    pktQueue->rear = newNode;
    pktQueue->size ++;

    pthread_cond_signal(&pktQueue->notEmtySignal);
    pthread_mutex_unlock(&pktQueue->mutex);

    //ALOGE("%s end ...",__func__ );
    return;
}

inline static void * deQueue(Queue *pktQueue)
{
    Node *tempNode ;
    void * data;

   // ALOGE("%s",__func__ );

    pthread_mutex_lock(&pktQueue->mutex);
    if ( pktQueue->size >= QUEUE_MAX ) {
        pthread_cond_signal(&pktQueue->FullSignal);
        if (debug)
            ALOGE("%s sent FullSignal ,size =%d ...",__func__ ,pktQueue->size);
    }

    while (pktQueue->front == NULL) {
        if (debug)
            ALOGE("%s front is NULL wait for notEmtySignal ...",__func__ );
        pthread_cond_wait(&pktQueue->notEmtySignal, &pktQueue->mutex);
    }

    tempNode = pktQueue->front;
    data = pktQueue->front->data;

    pktQueue->front = tempNode->next;

    if (pktQueue->front == NULL) {
        pktQueue->rear = NULL;
    }
    pktQueue->size--;
    pthread_mutex_unlock(&pktQueue->mutex);

    free(tempNode);

    //ALOGE("%s end",__func__ );
    return data;
}

#endif //JMAUDIOPLAYER_FFMPEGQUEUE_H
