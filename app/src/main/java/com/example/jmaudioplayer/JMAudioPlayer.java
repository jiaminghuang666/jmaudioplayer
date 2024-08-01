package com.example.jmaudioplayer;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

public class JMAudioPlayer {

    private static final String TAG = "JMAudioPlayer";
    private static final int MSG_START = 0;
    private static final int MSG_ERROR = 100;     /* arg1 = error */
    private static final int MSG_EOS   = 200;

    static {
        System.loadLibrary("jmaudioplayer");
    }


    public JMAudioPlayer (Context context)
    {
        //super(context);
        Log.d(TAG, " JMAudioPlayer class" );
        testFan();
    }

    private Handler mPlayerMsgHandler =new Handler(Looper.getMainLooper()) {
        public void handleMessage( Message msg) {
            switch (msg.what) {
                case MSG_START:
                    Log.d(TAG, " handleMessage MSG_START" );
                    break;
                case MSG_ERROR:
                    Log.d(TAG, " handleMessage MSG_ERROR" );
                    break;
                case MSG_EOS:
                    Log.d(TAG, " handleMessage MSG_EOS" );
                    break;

                default:
                    Log.d(TAG, " handleMessage default" );
                    break;
            }
        }
    };

     public void postEventFromNative(int id,int arg1, int arg2) {
         Log.d(TAG, " postEventFromNative " );
         if (mPlayerMsgHandler != null) {
             Message msg = new Message();
             msg.what = id;
             msg.obj = new Long(arg1);
             mPlayerMsgHandler.sendMessage(msg);
         }

     return ;
     }

    public native void testFan();
    public native long getParam(int id);
}
