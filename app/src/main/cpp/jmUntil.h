//
// Created by jiaming.huang on 2024/7/29.
//

#ifndef JMAUDIOPLAYER_JMUNTIL_H
#define JMAUDIOPLAYER_JMUNTIL_H

#include <list>
#include <mutex>
#include <thread>

using namespace std;

static void XSleep(int mis)
{
    chrono::milliseconds du(mis);
    this_thread::sleep_for(du);
}

#endif //JMAUDIOPLAYER_JMUNTIL_H
