package com.example.jmaudioplayer;

public class Constants {
    // handler msg
    public static final int MSG_MUSIC_INIT = 0;

    public static final int MSG_MUSIC_START = 1;

    public static final int MSG_MUSIC_PAUSE = 2;

    public static final int MSG_MUSIC_RESUME = 3;

    public static final int MSG_TOGGLE_MODE = 4;

    public static final int MSG_PLAY_COMPLETE = 5;




    // jni msg
    public static final int MSG_START = 0;
    public static final int MSG_ERROR = 100;     /* arg1 = error */
    public static final int MSG_INFO = 200;
    public static final int MSG_EOS   = 300;
}
