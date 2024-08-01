//
// Created by jiaming.huang on 2024/7/30.
//

#ifndef JMAUDIOPLAYER_JMAUDIOPLAYER_H
#define JMAUDIOPLAYER_JMAUDIOPLAYER_H

#define MSG_START                       0
#define MSG_ERROR                       100     /* arg1 = error */
#define MSG_EOS                         200


void jniPostEvent(int id, int arg1,int arg2);

#endif //JMAUDIOPLAYER_JMAUDIOPLAYER_H
