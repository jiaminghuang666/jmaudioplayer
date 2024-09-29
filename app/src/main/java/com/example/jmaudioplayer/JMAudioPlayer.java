package com.example.jmaudioplayer;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

public class JMAudioPlayer {

    private static final String TAG = "JMAudioPlayer";

    private Handler handler;

    private static long myDuration = 0;
    private static double myPosition = 0;


    static {
        System.loadLibrary("jmaudioplayer");
    }


    public JMAudioPlayer (Context context)
    {
        //super(context);
        Log.d(TAG, " JMAudioPlayer class" );
        testFan();
    }

    public void setHandler(Handler handler)
    {
        this.handler = handler;

    }

    public void playerstart(String Url) {
        int ret = 0;
        ret =  setDataSource(Url ,this); // "/data/guyongzhe.mp3"  "/data/1080.mp4" "/data/dukou.wav"
        if (ret != 0) {
            Log.d(TAG, " playerstart setDataSource =" + Url);
            return ;
        }
        prepareAsync();

        start();

        handler.sendEmptyMessage(Constants.MSG_MUSIC_INIT);
        handler.sendEmptyMessage(Constants.MSG_MUSIC_START);
        return;
    }

    public void playerStop() {
        stop();
        releaseSource();
        return;
    }

    public int playerPasue(boolean ispause) {
        int ret = 0;
        ret = pause(ispause);
        if (ret != 0) {
            Log.e(TAG, " playerPasue pause error !!" );
            return -1;
        }
        if (ispause) {
            handler.sendEmptyMessage(Constants.MSG_MUSIC_PAUSE);
        } else {
            handler.sendEmptyMessage(Constants.MSG_MUSIC_RESUME);
        }

        return 0;
    }

    public void playerseek() {
        seek();
        return;
    }

    public double  playergetCurrentPosition() {
        myPosition = getCurrentPosition() ;
        return myPosition;
    }

    public long playergetDuration() {
        myDuration = getDuration() / 1000;

        return myDuration;
    }


    private Handler mPlayerMsgHandler =new Handler(Looper.getMainLooper()) {
        public void handleMessage( Message msg) {
            switch (msg.what) {
                case Constants.MSG_START:
                    Log.d(TAG, " handleMessage MSG_START" );
                    break;
                case Constants.MSG_ERROR:
                    Log.d(TAG, " handleMessage MSG_ERROR" );
                    break;
                case Constants.MSG_EOS:
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


    public native void start();
    public native int setDataSource(String url,Object handle);
    public native int prepareAsync();

    public native int releaseSource();

    public native void stop();
    public native int pause(boolean isPause);
    public native void seek();
    public native double getCurrentPosition();
    public native long getDuration();

    public native void testFan();
    public native long getParam(int id);
}
