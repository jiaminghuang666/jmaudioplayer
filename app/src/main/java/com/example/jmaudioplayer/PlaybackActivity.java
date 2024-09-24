package com.example.jmaudioplayer;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.fragment.app.FragmentActivity;

/**
 * Loads {@link PlaybackVideoFragment}.
 */
public class PlaybackActivity extends FragmentActivity {

    private static final String TAG = "jiaming PlaybackActivity";
    private  Thread playbackThread = null;
    private  Thread playerInfoThread = null;
    private static JMAudioPlayer myJMAudioPlayer;

    private static long myDuration = 0;
    private static long myPosition = 0;

    //control bar button
    private LinearLayout controlBar = null;
    private ImageButton browserBtn = null;
    private ImageButton preBtn = null;
    private ImageButton fastreverseBtn = null;
    private ImageButton playBtn = null;
    private ImageButton nextBtn = null;
    private ImageButton fastforwordBtn = null;
    private ImageButton optBtn = null;
    private TextView totalTimeTx = null;
    private TextView curTimeTx = null;

    private int totaltime = 0;
    private int curtime = 0;


    static {
       System.loadLibrary("jmaudioplayer");
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_playback);


        initView();

        /*if (savedInstanceState == null) {
            getSupportFragmentManager()
                    .beginTransaction()
                    .replace(R.id.main_playback_fragment, new PlaybackVideoFragment())
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
        if (playerInfoThread ==null) {
            playerInfoThread = new Thread(playerInforrunnable);
            playerInfoThread.start();
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
            Log.d(TAG, " runnable " );
            ret =  setDataSource("/data/guyongzhe.mp3",this); // "/data/guyongzhe.mp3"  "/data/1080.mp4" "/data/dukou.wav"
            startPlay();

        }
    };

    private Runnable playerInforrunnable = new Runnable() {
        @Override
        public void run() {
            try {

                //curTimeTx.setText(myJMAudioPlayer.secToTime((int)myDuration));
               // Log.d(TAG, " playerInforrunnable  myDuration =" + myDuration );
                for(;;) {
                    Thread.sleep(1000);
                    myPosition = getCurrentPosition();
                    myDuration = getDuration();
                    //Log.d(TAG, " playerInforrunnable myPosition = " + myPosition +" " + "myDuration ="+myDuration );

                }
            }catch ( InterruptedException e) {
                e.printStackTrace();
            }
        }
    };


    private void initView() {
        controlBar = (LinearLayout) findViewById (R.id.infobarLayout);

        browserBtn = (ImageButton) findViewById (R.id.BrowserBtn);
        preBtn = (ImageButton) findViewById (R.id.PreBtn);
        fastforwordBtn = (ImageButton) findViewById (R.id.FastForwardBtn);
        playBtn = (ImageButton) findViewById (R.id.PlayBtn);
        fastreverseBtn = (ImageButton) findViewById (R.id.FastReverseBtn);
        nextBtn = (ImageButton) findViewById (R.id.NextBtn);
        optBtn = (ImageButton) findViewById (R.id.moreBtn);
        // controlBar.setVisibility (View.GONE);

        curTimeTx =(TextView) findViewById(R.id.CurTime);
        totalTimeTx = (TextView) findViewById(R.id.TotalTime);

       // curTimeTx.setText(myJMAudioPlayer.secToTime(totaltime));
       // totalTimeTx.setText(myJMAudioPlayer.secToTime(curtime));
        browserBtn.setOnClickListener (new ImageButton.OnClickListener() {
            public void onClick (View v) {
                Log.d (TAG, "browserBtn onClick");
                //browserBack();
            }
        });
        preBtn.setOnClickListener(new Button.OnClickListener() {
            public void  onClick (View v) {
                Log.d(TAG, "preBtn onClick !!");
            }
        });
        fastforwordBtn.setOnClickListener(new Button.OnClickListener() {
            public void  onClick (View v) {
                Log.d(TAG, "fastforwordBtn onClick !!");
            }
        });

        playBtn.setOnClickListener(new Button.OnClickListener(){
            public void  onClick (View v) {
                Log.d(TAG, "playBtn onClick !!");
            }
        });

        fastreverseBtn.setOnClickListener(new Button.OnClickListener(){
            public void  onClick (View v) {
                Log.d(TAG, "fastreverseBtn onClick !!");
            }
        });

        nextBtn.setOnClickListener(new Button.OnClickListener(){
            public void  onClick (View v) {
                Log.d(TAG, "nextBtn onClick !!");
            }
        });

        optBtn.setOnClickListener(new ImageButton.OnClickListener() {
            public void onClick (View v) {
                Log.d(TAG, "optBtn onClick !!");
            }
        });
    }





    public native int setDataSource(String url,Object handle);
   public native int releaseSource();

   public native void startPlay();
   public native void stopPlay();
   public native void pause();
   public native void seek();

   public native long getCurrentPosition();

   public native long getDuration();
}

