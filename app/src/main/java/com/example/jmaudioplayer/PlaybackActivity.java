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
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.fragment.app.FragmentActivity;

/**
 * Loads {@link PlaybackVideoFragment}.
 */
public class PlaybackActivity extends FragmentActivity implements SeekBar.OnSeekBarChangeListener{

    private static final String TAG = "jiaming PlaybackActivity";
    private  Thread playbackThread = null;
    private static JMAudioPlayer myJMAudioPlayer;

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

    private SeekBar seekPosition;

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

        myJMAudioPlayer.setHandler(handler);

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
        myJMAudioPlayer.playerStop();

        handler.removeCallbacksAndMessages(null);
        handler = null;
       // mPlayerMsgHandler.removeCallbacksAndMessages(null);
       // mPlayerMsgHandler = null;

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

        seekPosition = findViewById(R.id.SeekBar);
        seekPosition.setMax(100);
        seekPosition.setOnSeekBarChangeListener((SeekBar.OnSeekBarChangeListener)this);

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


    @Override
    public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
       // myJMplayer.Seek((double)seekBar.getProgress() / (double)seekBar.getMax());
    }



    private Runnable runnable = new Runnable() {
        @Override
        public void run() {
            myJMAudioPlayer.playerstart();
        }
    };

    private final Runnable playerInforrunnable = new Runnable() {
        @Override
        public void run() {
            try {
                    Thread.sleep(1000);
                    curTimeTx.setText(myJMAudioPlayer.secToTime((int)myJMAudioPlayer.playergetCurrentPosition()));
                    seekPosition.setProgress((int)((myJMAudioPlayer.playergetCurrentPosition() / myJMAudioPlayer.playergetDuration()) * 100) );
                    Log.d(TAG, " playerInforrunnable myPosition1 = " + myJMAudioPlayer.playergetCurrentPosition() +" " +
                            "myPosition2 ="+myJMAudioPlayer.secToTime((int)myJMAudioPlayer.playergetCurrentPosition()) );
                handler.postDelayed(this, 1000);
            }catch ( InterruptedException e) {
                e.printStackTrace();
            }
        }
    };

    public void updateView() {
        Log.d(TAG, " playerInforrunnable  myDuration =" + myJMAudioPlayer.playergetDuration() );
        totalTimeTx.setText(myJMAudioPlayer.secToTime((int)myJMAudioPlayer.playergetDuration()));
    }

private Handler handler = new Handler(Looper.getMainLooper()) {
    public void handleMessage(Message msg) {
        switch (msg.what) {
            case Constants.MSG_MUSIC_INIT:
                Log.d (TAG, "handleMessage MSG_MUSIC_INIT ");
                updateView();
                break;
            case Constants.MSG_MUSIC_START:
                Log.d (TAG, "handleMessage MSG_MUSIC_START ");
                handler.post(playerInforrunnable);
                break;
            case Constants.MSG_MUSIC_PAUSE:
                Log.d (TAG, "handleMessage MSG_MUSIC_PAUSE ");
                handler.removeCallbacks(playerInforrunnable);
                break;
            case Constants.MSG_TOGGLE_MODE:
                Log.d (TAG, "handleMessage MSG_TOGGLE_MODE ");
                break;
            case Constants.MSG_PLAY_COMPLETE:
                Log.d (TAG, "handleMessage MSG_PLAY_COMPLETE ");
                break;
            default:
                break;
        }
    }
};

}

