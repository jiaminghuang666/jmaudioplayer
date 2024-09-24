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


    private  String unitFormat(int i) {
        String reStr = null;
        if (i >= 0 && i < 10) {
            reStr = "0" + Integer.toString(i);
        } else {
            reStr =  Integer.toString(i);
        }
        return reStr;
    }

    public String secToTime(int i) {
        String retStr = null;
        int hour = 0;
        int minute = 0;
        int second = 0;

        if (i <= 0) {
            return "00:00:00";
        }else {
            minute = i / 60;
            if (minute < 60) {
                second = i % 60;
                retStr = "00:" + unitFormat(minute) + ":" + unitFormat(second);
            } else {
                hour = minute / 60;
                if (hour > 99) return "99:59:59";
                minute = minute % 60;
                second = i % 60;
                retStr = unitFormat(hour) + ":" + unitFormat(minute) + ":" + unitFormat(second);
            }
        }
        Log.d(TAG, " secToTime retStr =" + retStr );
        return retStr;
    }

}
