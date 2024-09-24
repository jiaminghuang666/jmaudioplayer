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

    public static final int MSG_MUSIC_INIT = 0;

    public static final int MSG_MUSIC_START = 1;

    public static final int MSG_MUSIC_PAUSE = 2;

    public static final int MSG_TOGGLE_MODE = 3;

    public static final int MSG_PLAY_COMPLETE = 4;




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
        handler.removeCallbacksAndMessages(null);
        handler = null;
       // mPlayerMsgHandler.removeCallbacksAndMessages(null);
       // mPlayerMsgHandler = null;
        releaseSource();
    }

    protected void onRestart() {
        super.onRestart();
        Log.d(TAG, "onRestart");
    }

    private void initView() {
        Log.d (TAG, "browserBtn onClick");
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


    private Runnable runnable = new Runnable() {
        @Override
        public void run() {
            int ret = 0 ;
            Log.d(TAG, " runnable " );
            playerStart();
        }
    };

    private final Runnable playerInforrunnable = new Runnable() {
        @Override
        public void run() {
            Log.d(TAG, " playerInforrunnable  run " );
            try {
                //for(;;) {
                    Thread.sleep(1000);

                    playergetCurrentPosition();
                    playergetDuration();
                    //Log.d(TAG, " playerInforrunnable  myDuration =" + temp );
                    totalTimeTx.setText("11:11");
                    //Log.d(TAG, " playerInforrunnable myPosition = " + myPosition +" " + "myDuration ="+myDuration );

                //}
            }catch ( InterruptedException e) {
                e.printStackTrace();
            }
        }
    };


private Handler handler = new Handler(Looper.getMainLooper()) {
    public void handleMessage(Message msg) {
        switch (msg.what) {
            case MSG_MUSIC_INIT:
                Log.d (TAG, "MSG_MUSIC_INIT onClick");
                break;
            case MSG_MUSIC_START:
                Log.d (TAG, "MSG_MUSIC_START onClick");
                handler.post(playerInforrunnable);
                break;
            case MSG_MUSIC_PAUSE:
                Log.d (TAG, "MSG_MUSIC_PAUSE onClick");
                handler.removeCallbacks(playerInforrunnable);
                break;
            case MSG_TOGGLE_MODE:
                Log.d (TAG, "MSG_TOGGLE_MODE onClick");
                break;
            case MSG_PLAY_COMPLETE:
                Log.d (TAG, "MSG_PLAY_COMPLETE onClick");
                break;
            default:
                break;
        }
    }
};


    public void playerStart() {
        Log.d (TAG, "playerStart");
        int ret = 0;

        ret =  setDataSource("/data/guyongzhe.mp3",this); // "/data/guyongzhe.mp3"  "/data/1080.mp4" "/data/dukou.wav"
        startPlay();
        handler.sendEmptyMessage(MSG_MUSIC_START);

        return ;
    }

    public void playerStop() {
        stopPlay();
        return;
    }

    public void playerPasue() {
        pause();

        return;
    }

    public void playerseek() {
        seek();

        return;
    }

    public void playergetCurrentPosition() {
        myPosition = getCurrentPosition();

        return;
    }

    public void playergetDuration() {
        myDuration = getDuration();

        return;
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

