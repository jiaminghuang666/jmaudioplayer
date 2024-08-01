package com.example.jmaudioplayer;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import androidx.fragment.app.FragmentActivity;

/**
 * Loads {@link PlaybackVideoFragment}.
 */
public class PlaybackActivity extends FragmentActivity {

    private static final String TAG = "jiaming PlaybackActivity";
    private  Thread playbackThread = null;
    private static JMAudioPlayer myJMAudioPlayer;

    static {
       System.loadLibrary("jmaudioplayer");
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        /*if (savedInstanceState == null) {
            getSupportFragmentManager()
                    .beginTransaction()
                    .replace(android.R.id.content, new PlaybackVideoFragment())
                    .commit();
        }*/
        myJMAudioPlayer = new JMAudioPlayer(this);
    }

    @Override
    protected void onStart(){
        super.onStart();
        Log.d(TAG, "onStart");
        if (playbackThread == null) {
            playbackThread = new Thread(runnable);
            playbackThread.start();
        }
    }
    @Override
    protected void onResume(){
        super.onResume();
        Log.d(TAG, "onResume");
    }

    @Override
    protected void onPause(){
        super.onPause();
        Log.d(TAG, "onPause");
    }

    protected void onStop() {
        super.onStop();
        Log.d(TAG, "onStop");
        //releaseSource();
    }

    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy");
       // mPlayerMsgHandler.removeCallbacksAndMessages(null);
       // mPlayerMsgHandler = null;
        releaseSource();
    }

    protected void onRestart() {
        super.onRestart();
        Log.d(TAG, "onRestart");
    }

    private Runnable runnable = new Runnable() {
        @Override
        public void run() {
            int ret = 0 ;
            Log.d(TAG, " PlaybackActivity " );
           ret =  setDataSource("/data/guyongzhe.mp3",this); // "/data/guyongzhe.mp3"  "/data/1080.mp4" "/data/dukou.wav"
            startPlay();
        }
    };


    public native int setDataSource(String url,Object handle);
   public native int releaseSource();

   public native void startPlay();
   public native void stopPlay();
   public native void pause();

}
