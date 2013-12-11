package com.ford.avarsdl.views;

import java.io.File;
import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.app.DialogFragment;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.graphics.Rect;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.StrictMode;
import android.preference.PreferenceManager;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings.RenderPriority;
import android.webkit.WebView;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;
import android.widget.Toast;

import com.android.vending.expansion.zipfile.APKExpansionSupport;
import com.android.vending.expansion.zipfile.ZipResourceFile;

import com.ford.avarsdl.R;
import com.ford.avarsdl.downloader.DownloaderClient;
import com.ford.avarsdl.downloader.XAPKFile;
import com.ford.avarsdl.exception.ExtensionFileException;
import com.ford.avarsdl.exception.MediaPlayerException;
import com.ford.avarsdl.manager.RPCComponentsManager;
import com.ford.avarsdl.rater.AppRater;
import com.ford.avarsdl.service.ISDLServiceConnection;
import com.ford.avarsdl.service.SDLService;
import com.ford.avarsdl.service.SDLServiceBinder;
import com.ford.avarsdl.service.SDLServiceConnectionProxy;
import com.ford.avarsdl.util.ActivityUtils;
import com.ford.avarsdl.util.Const;
import com.ford.avarsdl.util.ExtStorageUtils;
import com.ford.avarsdl.util.Logger;
import com.ford.avarsdl.util.MessageConst;
import com.ford.avarsdl.util.Utils;
import com.ford.avarsdl.util.WebViewUtils;
import com.ford.syncV4.proxy.interfaces.IProxyListenerALM;

/**
 * Title: AvatarActivity.java<br>
 * Description: Main application activity, responsible for webview content load
 * and video handling<br>
 *
 * @author vsaenko/Eugene Sagan
 * @co-author Yuriy Chernyshov
 */
public class AvatarActivity extends Activity implements SurfaceHolder.Callback,
        ISDLServiceConnection {

    private final static String APP_SETUP_DIALOG_TAG = "AppSetupDialogTag";
    private final SDLServiceConnectionProxy mSDLServiceConnectionProxy = new SDLServiceConnectionProxy(this);
    private IProxyListenerALM mBoundSDLService;

    // for monkey testing
    // adb shell monkey -p com.ford.avarsdl -v 100

    public static Boolean fullscreenPreferenceChanged = false;
    public static Boolean vehiclePreferenceChanged = false;
    public static Boolean navigationPreferenceChanged = false;

    public static Boolean ratePreferenceEnabled;
    private RPCComponentsManager mRPCComponentsManager = new RPCComponentsManager();

    @Override
    public void onStart() {
        super.onStart();

        Logger.i(getClass().getSimpleName() + " onStart ");

        if (mBoundSDLService != null) {

        } else {
            bindSDLService(this, mSDLServiceConnectionProxy);
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Logger.i(getClass().getSimpleName() + " onCreate, hash:" + hashCode());

        // FIXME: the old code with new SDK crashes with
        // android.os.NetworkOnMainThreadException
        // this hotfix is a bad idea. Don't try this at home!
        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

        if (isFirstStart()) {
            Intent intent = new Intent(this, EulaActivity.class);
            startActivityForResult(intent, Const.REQUESTCODE_EULA);
        } else {
            startAvatarActivity();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == RESULT_OK && requestCode == Const.REQUESTCODE_EULA) {
            startAvatarActivity();
            resumeAvatarActivity();
        } else {
            finish();
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if ((keyCode == KeyEvent.KEYCODE_BACK) && mWebView.canGoBack()) {
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public void onBackPressed() {
        // do something on back.
        this.moveTaskToBack(true);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.optionsmenu, menu);
        return true;
    }

    public void bindSDLService(Context context, SDLServiceConnectionProxy connectionProxy) {
        Logger.i("BindStorageService(), connection proxy: " + connectionProxy);
        context.bindService(new Intent(context, SDLService.class), connectionProxy, BIND_AUTO_CREATE);
    }

    public void unbindSDLService(Context context, SDLServiceConnectionProxy connectionProxy) {
        if (!connectionProxy.isConnected()) {
            Logger.v("ServiceConnection is not connected, ignoring unbindService: " + connectionProxy);
            return;
        }
        try {
            Logger.i("Unbind Service(), connection proxy: " + connectionProxy);
            context.unbindService(connectionProxy);
        } catch (IllegalArgumentException iae) {
            // sometimes this exception is still thrown, in spite of isConnected() check above
            // simply ignore this exception
            Logger.w("Unbind IllegalArgumentException: " + iae);
        } catch (Exception e) {
            Logger.e("Error unbinding from connection: " + connectionProxy, e);
        }
    }


    public boolean onOptionsItemSelected(MenuItem item) {
        final int itemId = item.getItemId();
        switch (itemId) {
            case R.id.mnuQuit:
                exitApp();
                break;
            default:
                break;
        }
        return false;
    }

    private void exitApp() {
        Logger.d("Exiting application");

        finish();
        // the delay should be long enough, so that UnregisterAppInterface and
        // EndSession messages are sent
        new Timer().schedule(new TimerTask() {
            @Override
            public void run() {
                android.os.Process.killProcess(android.os.Process.myPid());
            }
        }, 2000);
    }

    public boolean isFirstStart() {
        // Commit according to REVSDL-116

        /*SharedPreferences prefs = getSharedPreferences(Const.SHPREF_FIRST_LAUNCH, 0);
        int previousCodeVersion = prefs.getInt(Const.SHPREF_PREVIOUS_CODE_VERSION, 0);
        int currentCodeVersion = Utils.getAppVersionCode(this);
        return previousCodeVersion < currentCodeVersion;*/

        return false;
    }

    public void surfaceCreated(SurfaceHolder holder) {
        Logger.i("surface created");
        playVideo(holder);
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        Logger.i("surface destroyed");
    }

    // =============================================================
    // GETTERS
    // =============================================================
    public WebView getWebView() {
        return mWebView;
    }

    public MediaPlayer getMediaPlayer() {
        return mMediaPlayer;
    }

    // get value from application settings
    public Boolean getFullscreenStatus() {
        return mPreferences.getBoolean("FULLSCREEN", true);
    }

    // get value from application settings
    public Boolean getMapsStatus() {
        return mPreferences.getBoolean("MAPS", true);
    }

    // get value from application settings
    public String getVehicleStatus() {
        return mPreferences.getString("VEHICLES", "Ford");
    }

    // get value from application settings
    public Boolean getRateStatus() {
        return mPreferences.getBoolean("RATE", true);
    }

    // set value to application settings
    public Boolean setMapsStatus(boolean value) {
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putBoolean("MAPS", value);
        editor.commit();
        return true;
    }

    // set value to application settings
    public boolean setVehicleStatus(String value) {
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putString("VEHICLES", value);
        editor.commit();
        return true;
    }

    public double audioGetPosition() {
        return mediaGetPosition();
    }

    public String getPathToDownloadedAudioFile(String fileName) {
        final File file = new File(getExternalFilesDir(null), fileName + ".mp3");
        return file.getAbsolutePath();
    }

    public Handler getMainHandler() {
        return handler;
    }

    public int getVideoPausedPosition() {
        return mVideoPauseTime;
    }

    public double videoGetPosition() {
        return mediaGetPosition();
    }

    public boolean videoWasPaused() {
        return mVideoWasPaused;
    }

    public boolean getVideoPlayed() {
        return mVideoPlayed;
    }

    public boolean getVideoPrepared() {
        return mVideoPrepared;
    }

    // setters

    public void setVideoPlayed(boolean value) {
        mVideoPlayed = value;
    }

    public void setVideoPrepared(boolean value) {
        mVideoPrepared = value;
    }

    // ==============================================================
    // OTHER METHODS
    // ==============================================================

    public void startVideoTimer() {
        // start timer to send every N ms current time position of video
        Logger.i("Start Timer");
        int N = 500;

        stopVideoTimer();

        mVideoTimer = new Timer();
        TimerTask timerTask = new TimerTask() {
            @Override
            public void run() {
                // current position in MP never reaches duration value
                // check difference on 500 msec
                if (mVideoPrepared) {
                    int position;
                    if ((mMediaPlayer.getDuration() - mMediaPlayer.getCurrentPosition()) < 500)
                        position = mMediaPlayer.getDuration();
                    else
                        position = mMediaPlayer.getCurrentPosition();
                    mRPCComponentsManager.sendPositionNotificationToVideoController(position);
                }
            }
        };
        mVideoTimer.schedule(timerTask, N, N);
    }

    public void setProgresbarVisibility(boolean value) {
        int v = value ? ProgressBar.VISIBLE : ProgressBar.GONE;
        mProgressBar.setVisibility(v);
    }

    // =====================================================================
    // private section
    // =====================================================================

    // private Context ctx;

    private MediaPlayer mMediaPlayer;

    private SharedPreferences mPreferences;
    private WebView mWebView;
    private SurfaceView mPreview;
    private SurfaceHolder mHolder;
    private ProgressBar mProgressBar;
    private RelativeLayout mVideoLayout;
    private ImageView mLogo;

    private int mSeekTo;

    private int mVideoWidth;
    private int mVideoHeight;
    private int mLeftMargin;
    private int mTopMargin;

    private String mPath;

    private Animation mAnimationShow;

    // for video time visualization
    private Timer mVideoTimer;// timer to send notifications about current video
    // time
    private boolean mVideoWasPaused = false;// indicate if video was on paused
    // before starting activity
    private int mVideoPauseTime = 0;// time in msec when video was paused before
    // starting activity
    private boolean mVideoPlayed = false;// indicate if video played before
    // starting activity
    // to measure 5 seconds for splash screen;
    private long mStartTime;
    private boolean mVideoPrepared = false; // indicate that video in MP is
    // prepared and timer can be started

    private AppRater mAppRater;

    private ZipResourceFile apkExpansionZipFile = null;
    private Context mContext;

    private final Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {

            super.handleMessage(msg);

            switch (msg.what) {

                // /// VIDEO
                case Const.VIDEO_START:
                    Logger.i("mMediaPlayer.play handler");
                    Object[] obj = (Object[]) msg.obj;
                    String videoFile = (String) obj[0];
                    Double scale = (Double) obj[1];
                    int x = (Integer) obj[2];
                    int y = (Integer) obj[3];
                    videoViewResize(scale, y, x);
                    mPath = videoFile;
                    initVideoSurface();
                    break;
                case Const.VIDEO_PAUSE:
                    Logger.i("mMediaPlayer.pause handler");
                    stopVideoTimer();
                    if (mMediaPlayer != null)
                        mMediaPlayer.pause();
                    mVideoWasPaused = true;
                    mVideoPauseTime = mMediaPlayer.getCurrentPosition();
                    break;
                case Const.VIDEO_SET_POSITION:
                    Double newPos = (Double) msg.obj * 1000;
                    newPos = newPos == 0 ? 1 : newPos; // because media player can
                    // not set zero value
                    if (mMediaPlayer != null)
                        mediaSetPosition(newPos);
                    break;
                case Const.VIDEO_SET_POSITION_PAUSED:
                    newPos = (Double) msg.obj * 1000;
                    newPos = newPos == 0 ? 1 : newPos; // because media player can
                    // not set zero value
                    if (mMediaPlayer != null)
                        mediaSetPositionPaused(newPos);
                    break;
                case Const.VIDEO_PLAY:
                    startVideoTimer();
                    mVideoWasPaused = false;
                    Logger.i("mMediaPlayer.start case Const.VIDEO_PLAY");
                    if (null != mMediaPlayer) {
                        mMediaPlayer.start();
                    } else {
                        Logger.i("mMediaPlayer == null");
                    }
                    break;
                case Const.VIDEO_STOP:
                    mVideoLayout.setVisibility(View.INVISIBLE);
                    stopVideoTimer();
                    releaseMediaPlayer();
                    mVideoWidth = 0;
                    mVideoHeight = 0;
                    mLeftMargin = 0;
                    mTopMargin = 0;
                    mPath = null;
                    mVideoWasPaused = false;
                    mVideoPlayed = false;
                    mVideoPauseTime = 0;
                    break;
                case Const.VIDEO_PLAY_AFTER_SCALE:
                    Logger.i("mMediaPlayer.play after scale handler");
                    obj = (Object[]) msg.obj;
                    scale = (Double) obj[0];
                    int left = (Integer) obj[1];
                    int top = (Integer) obj[2];
                    videoViewResize(scale, top, left);
                    initVideoSurface();
                    break;

                case Const.CONTENT_CHECKER_START:
                    verifyContent();
                    break;

                case Const.WEBVIEW_SHOW:
                    hideSplashScreen();
                    setProgresbarVisibility(false);
                    mWebView.startAnimation(mAnimationShow);
                    mWebView.setVisibility(View.VISIBLE);
                    if (Const.DEBUG) {
                        long currTime = System.currentTimeMillis();
                        String str = "Application loading time = " + String.valueOf(currTime - mStartTime);
                        Logger.i(str);
                        SafeToast.showToastAnyThread(str);
                    }
                    DialogFragment appSetupDialogFragment = AppSetupDialog.newInstance(R.string.app_setup_dialog_title);
                    appSetupDialogFragment.show(getFragmentManager(), APP_SETUP_DIALOG_TAG);
                    appSetupDialogFragment.setCancelable(false);
                    break;

                default:
                    break;
            }
        }
    };

    private void verifyContent() {

        removeOldVideoFiles();

        // check main expansion file
        int mainVersionCode = expFileIsDelivered(true);

        SharedPreferences prefs = getSharedPreferences(
                Const.SHPREF_DOWNLOADER_PREFS, 0);
        boolean mainMD5IsCalculated = prefs.getBoolean(
                Const.SHPREF_MAIN_EXPFILE_VALID, false);

        // check patch expansion file
        int patchVersionCode = expFileIsDelivered(false);

        boolean patchMD5IsCalculated = prefs.getBoolean(
                Const.SHPREF_PATCH_EXPFILE_VALID, false);

        if (mainVersionCode > 0 && mainMD5IsCalculated
                && (patchVersionCode > 0 || Const.PATCH_EXP_FILE_SIZE == 0)
                && patchMD5IsCalculated) {
            try {
                apkExpansionZipFile = APKExpansionSupport
                        .getAPKExpansionZipFile(this, mainVersionCode,
                                patchVersionCode);
            } catch (IOException e) {
                Logger.e(e.getMessage(), e);
            }

        } else {
            // no expansion file
            SharedPreferences.Editor editor = prefs.edit();
            if (editor != null) {
                editor.putBoolean(Const.SHPREF_MAIN_EXPFILE_VALID, false);
                editor.putBoolean(Const.SHPREF_PATCH_EXPFILE_VALID, false);
            }
            editor.commit();
            // create downloader client
            new DownloaderClient(this, mainVersionCode, mainMD5IsCalculated,
                    patchVersionCode, patchMD5IsCalculated);

        }

        Utils.sendMessageToHandler(Const.WEBVIEW_SHOW,
                MessageConst.NET_DOWNLOAD_FINISHED_SUCC, handler);
    }

    /**
     * checks if expansion file was downloaded previously
     *
     * @param isMain - is a main expansion file or a patch
     * @return version code if file exists or 0
     */

    private int expFileIsDelivered(boolean isMain) {
        int codeVersion = Utils.getAppVersionCode(getApplicationContext());
        long size = isMain ? Const.MAIN_EXP_FILE_SIZE
                : Const.PATCH_EXP_FILE_SIZE;
        while (codeVersion > 0) {
            XAPKFile xFile = new XAPKFile(isMain, codeVersion, size);
            if (ExtStorageUtils.expansionFilesDelivered(mContext, xFile)) {
                return codeVersion;
            }
            codeVersion--;
        }
        return codeVersion;
    }

    public void setExpantionFile(ZipResourceFile file) {
        apkExpansionZipFile = file;
    }

    /**
     * removes video files of MFTG 2, that is apart of expansion file
     */
    private boolean removeOldVideoFiles() {
        File oldVideoFilesDirectory = getExternalFilesDir(null);
        if ((oldVideoFilesDirectory != null) &&
                oldVideoFilesDirectory.exists()) {
            boolean success = true;
            final File[] files = oldVideoFilesDirectory.listFiles();
            if (files != null) {
                for (File file : files) {
                    success &= file.delete();
                }
                return success;
            }
        }
        Logger.i("Couldn't remove old video files");
        return false;
    }

    @Override
    protected void onPause() {
        super.onPause();
        Logger.i(getClass().getSimpleName() + " onPause, hash:" + hashCode());
        if (!isFirstStart()) {
            ActivityUtils.setAppIsForeground(false);
            // switch of timer
            stopVideoTimer();
            // remember last video position
            if (mMediaPlayer != null)
                mVideoPauseTime = mMediaPlayer.getCurrentPosition();
            // delete media player
            releaseMediaPlayer();
            mVideoLayout.removeAllViews();
        }
    }

    protected void onResume() {
        super.onResume();
        Logger.i(getClass().getSimpleName() + " onResume, hash:" + hashCode());
        if (!isFirstStart()) {
            resumeAvatarActivity();
        }
    }

    private void startAvatarActivity() {
        mStartTime = System.currentTimeMillis();
        mContext = this;
        // Possible work around for market launches. See
        // http://code.google.com/p/android/issues/detail?id=2373
        // for more details. Essentially, the market launches the main activity
        // on top of other activities.
        // we never want this to happen. Instead, we check if we are the root
        // and if not, we finish.
        if (!isTaskRoot()) {
            final Intent intent = getIntent();
            final String intentAction = intent.getAction();
            if (intent.hasCategory(Intent.CATEGORY_LAUNCHER) && intentAction != null
                    && intentAction.equals(Intent.ACTION_MAIN)) {
                Logger.w("Main Activity is not the root. Finishing Main Activity instead of launching.");
                finish();
                return;
            }
        }

        mAppRater = new AppRater(this);

        prepareMainView();
        WebViewUtils.initWebView(this);
        initVideoSurface();

        mRPCComponentsManager.setCallback(new RPCComponentsManager.RPCComponentsManagerCallback() {
            @Override
            public void onComplete() {
                Logger.i("RPC Components running successfully");
                // TODO: Implement Notification here
            }

            @Override
            public void onError(String message) {
                Logger.e("Can not run RPC Components: " + message);
                // TODO: Implement Notification here
            }
        });
        mRPCComponentsManager.init(this);
    }

    private void resumeAvatarActivity() {
        ActivityUtils.setAppIsForeground(true);

        if (!fullscreenPreferenceChanged && !vehiclePreferenceChanged)
            initVideoSurface(); // to init video after resuming but not
                                // after FullScreen or Vehicle changing

        if (mWebView.getUrl() == null) {
            if (!loadContent()) {
                Toast.makeText(this, R.string.toast_index_not_found, Toast.LENGTH_LONG).show();
            }
        }

        if (mRPCComponentsManager.getBackendController() == null) {
            return;
        }

        if (fullscreenPreferenceChanged) {
            fullscreenPreferenceChanged = false;
            mRPCComponentsManager.getBackendController().sendFullScreenRequest(getFullscreenStatus());
        }

        if (vehiclePreferenceChanged) {
            vehiclePreferenceChanged = false;
            mRPCComponentsManager.getBackendController().sendVehicleNotification(getVehicleStatus());
        }

        if (navigationPreferenceChanged) {
            navigationPreferenceChanged = false;
            mRPCComponentsManager.getBackendController().sendHasMapsNotification(getMapsStatus());
        }
    }

    @Override
    protected void onDestroy() {
        Logger.i(getClass().getSimpleName() + " onDestroy, hash:" + hashCode());

        unbindSDLService(getBaseContext(), mSDLServiceConnectionProxy);
        stopService(new Intent(this, SDLService.class));

        if (!isFirstStart()) {
            // switch of timer
            stopVideoTimer();

            releaseMediaPlayer();
            doCleanUp();
            mRPCComponentsManager.close();
        }
        super.onDestroy();
    }

    /**
     * Checks if the filename exists in the assets.
     *
     * @param assetFilename Filename without any prefix
     * @return true if the file exists
     */
    private boolean assetExists(final String assetFilename) {
        AssetManager am = getResources().getAssets();
        try {
            am.open(assetFilename);
        } catch (IOException e) {
            return false;
        }
        return true;
    }

    private boolean loadContent() {
        Logger.i("loadContent");
        boolean successful = false;
        if (assetExists(Const.INDEX_PAGE)) {
            mWebView.loadUrl(Const.WEB_MAIN_PAGE_PATH);
            successful = true;
        }
        return successful;
    }

    private void prepareMainView() {
        Logger.i("prepareMainView");
        setContentView(R.layout.main);

        mPreferences = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        Boolean fullScreen = getFullscreenStatus();
        mLogo = (ImageView) findViewById(R.id.logo);
        Logger.i("mFullScreen is " + fullScreen.toString());
        if (fullScreen) {
            RelativeLayout.LayoutParams params = new LayoutParams(
                    LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT);
            mLogo.setLayoutParams(params);
            // mLogo.setLayoutParams(params);
        } else {

            RelativeLayout.LayoutParams params = new LayoutParams(800, 480);
            params.addRule(RelativeLayout.CENTER_HORIZONTAL);
            params.addRule(RelativeLayout.CENTER_VERTICAL);
            mLogo.setLayoutParams(params);
        }
        mWebView = (WebView) findViewById(R.id.webView1);
        mWebView.setWebChromeClient(new WebChromeClient());
        mWebView.getSettings().setRenderPriority(RenderPriority.HIGH);
        // mWebView.getSettings().setLoadWithOverviewMode(true);
        // mWebView.getSettings().setUseWideViewPort(true);
        // mWebView.setInitialScale(10);

        mVideoLayout = (RelativeLayout) findViewById(R.id.videoLayout);
        // set progress bar
        mProgressBar = (ProgressBar) findViewById(R.id.pbProgress);
        LayoutParams progressParams = (LayoutParams) mProgressBar
                .getLayoutParams();
        double loaderHeight = Const.WEB_HEIGHT * getScale() * 0.07 * getWindowDensity();
        progressParams.height = (int) loaderHeight;
        progressParams.width = progressParams.height;
        double loaderVerticalCenter = getWindowHeight() * 0.5;
        double loaderVerticalShift = Const.WEB_HEIGHT * getScale() * 0.5 * 0.41 * getWindowDensity();
        double top = (loaderVerticalCenter - 0.5 * loaderHeight) + loaderVerticalShift;
        progressParams.topMargin = (int) Math.round(top);
        mProgressBar.setLayoutParams(progressParams);

        mAnimationShow = AnimationUtils.loadAnimation(this, R.anim.show);
    }

    private double getScale() {
        double res = 1;
        if (isFullScreen()) {
            /** Calculate Scale Point */
            double scalePointW = (double) getWindowWidth()
                    / (double) Const.WEB_WIDTH / getWindowDensity();
            double scalePointH = (double) getWindowHeight()
                    / (double) Const.WEB_HEIGHT / getWindowDensity();
            /** Set calculated ScalePoint */
            res = (scalePointW >= scalePointH) ? scalePointH : scalePointW;
        } else {
            res /= getWindowDensity();
        }
        return res;
    }

    private boolean isFullScreen() {
        SharedPreferences preferences = PreferenceManager
                .getDefaultSharedPreferences(this.getApplicationContext());
        return preferences.getBoolean("FULLSCREEN", true);
    }

    private Integer getWindowHeight() {
        Integer height;
        if ((Build.VERSION.SDK_INT > 10)/* && (Build.VERSION.SDK_INT < 14) */) {
            // return height without tabs bar
            Rect rectgle = new Rect();
            Window window = this.getWindow();
            window.getDecorView().getWindowVisibleDisplayFrame(rectgle);
            height = rectgle.bottom; // height = - panel height
        } else {
            DisplayMetrics dm = new DisplayMetrics();
            this.getWindowManager().getDefaultDisplay().getMetrics(dm);
            if (dm.heightPixels > dm.widthPixels) {
                height = dm.widthPixels;
            } else {
                height = dm.heightPixels;
            }
        }
        return height;
    }

    private Integer getWindowWidth() {
        Integer width;
        DisplayMetrics dm = new DisplayMetrics();
        this.getWindowManager().getDefaultDisplay().getMetrics(dm);
        if (dm.heightPixels > dm.widthPixels) {
            width = dm.heightPixels;
        } else {
            width = dm.widthPixels;
        }
        return width;
    }

    private void initVideoSurface() {
        Logger.i("init surface");

        mVideoLayout.setVisibility(View.INVISIBLE);
        mVideoLayout.removeAllViews();

        LayoutParams params = (LayoutParams) mVideoLayout.getLayoutParams();

        params.height = mVideoHeight;
        params.width = mVideoWidth;
        params.leftMargin = mLeftMargin;
        params.topMargin = mTopMargin;

        mVideoLayout.setLayoutParams(params);
        mPreview = new SurfaceView(this);

        mVideoLayout.addView(mPreview);
        mHolder = mPreview.getHolder();
        mHolder.addCallback(this);
        mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        mVideoLayout.forceLayout();
        mVideoLayout.setVisibility(View.VISIBLE);

    }

    private void hideSplashScreen() {
        Logger.i("hide screen ");
        mVideoLayout.setVisibility(View.VISIBLE);
        mLogo.setVisibility(View.INVISIBLE);
    }

    private double mediaGetPosition() {
        // setIsGetPosition(true);
        Logger.i("mediaGetPosition");
        // isDoubleSeek = true;
        if (mMediaPlayer == null) {
            return -1;
        }
        try {
            return Utils
                    .getShortDouble(mMediaPlayer.getCurrentPosition() / 1000.0);
        } catch (IllegalStateException e) {
            Logger.e(e.getMessage(), e);
            return -1;
        }
    }

    private void mediaSetPosition(double position) {
        /* convert to milliseconds */
        // setIsGetPosition(false);
        // position *= 1000;
        mSeekTo = (int) position;
        Logger.i("mediaSetPosition=" + position);

        try {
            if (mSeekTo != 0)
                mMediaPlayer.seekTo(mSeekTo);
        } catch (IllegalStateException e) {
            Logger.e(e.getMessage(), e);
        }
    }

    private void mediaSetPositionPaused(double position) {
		/* convert to milliseconds */
        mSeekTo = (int) position;

        Logger.i("mediaSetPositionPaused=" + position);

        AudioManager audioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        audioManager.setStreamMute(AudioManager.STREAM_MUSIC, true);
        try {
            mMediaPlayer.seekTo(mSeekTo);
        } catch (IllegalStateException e) {
            Logger.e(e.getMessage(), e);
        }
    }

    private void releaseMediaPlayer() {
        if (mMediaPlayer != null) {
            mMediaPlayer.reset();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
        mVideoPrepared = false;
    }

    private void videoViewResize(double scale, int top, int left) {
        mVideoWidth = (int) (Const.ORIG_VIDEO_WIDTH * scale * getWindowDensity() - 1);
        mVideoHeight = (int) (Const.ORIG_VIDEO_HEIGHT * scale * getWindowDensity() - 1);
        mLeftMargin = (int) Math.round(left * getWindowDensity());
        mTopMargin = (int) Math.round(top * getWindowDensity());
    }

    private double getWindowDensity() {
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);
        return dm.density;
    }

    private void playVideo(SurfaceHolder holder) {
        Logger.i("playVideo");
        if (mPath != null) {
            try {
                if (mMediaPlayer != null) {
                    releaseMediaPlayer();
                }
                // TODO : optimize MP instantiation
                // Create a new media player and set the listeners
                mMediaPlayer = new MediaPlayer();

                initDatasourceWith(mPath);

                Logger.i("video file path : " + mPath);

                mMediaPlayer.setDisplay(holder);

                mMediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
                    @Override
                    public void onPrepared(MediaPlayer mediaPlayer) {
                        Logger.d("MediaPlayer onPrepared -> Play");
                        try {
                            if (getVideoPausedPosition() > 0)
                                mediaPlayer.seekTo(getVideoPausedPosition());
                            mediaPlayer.start();
                            //set duration of video
                            Log.i(Const.APP_TAG, "OnPrepareListener GetDuration: " +
                                    mediaPlayer.getDuration() + " ms");
                            mRPCComponentsManager.setVideoControllerDuration(mediaPlayer.getDuration());
                            setVideoPlayed(true);
                            setVideoPrepared(true);
                            if (videoWasPaused())
                                mediaPlayer.pause();
                            else
                                startVideoTimer();

                        } catch (IllegalStateException e) {
                            Log.e(Const.APP_TAG, e.getMessage(),e);
                        }
                    }
                });
                mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);

                mMediaPlayer.prepare();
            } catch (Exception e) {
                Logger.e("error: " + e.getMessage(), e);
                // set duration of video
                mRPCComponentsManager.setVideoControllerDuration(-1);
            }
        } else {
            Logger.i("No video to play");
        }
    }

    private void stopVideoTimer() {
        if (mVideoTimer != null) {
            mVideoTimer.cancel();
            mVideoTimer.purge();
        }
        mVideoTimer = null;
    }

    private void doCleanUp() {
        mVideoWidth = 0;
        mVideoHeight = 0;
        if (mVideoLayout != null)
            mVideoLayout.removeAllViews();
        mHolder = null;
        mPreview = null;
        mVideoWasPaused = false;
        mVideoPauseTime = 0;
        mVideoPlayed = false;
    }

    private void initDatasourceWith(String fileName)
            throws ExtensionFileException, MediaPlayerException {

        AssetFileDescriptor assetFileDescriptor;
        // welcome orientation video is in raw resources
        if (fileName.toLowerCase().compareTo(Const.WELCOME_VIDEO_FILE_NAME) == 0) {

            // Commit according to REVSDL-116
            //assetFileDescriptor = getResources().openRawResourceFd(R.raw.faq_welcome_orientation);
            Logger.w("Welcome Video is disabled in current version");
            return;

        } else {
            // get file from expansion archive
            if (apkExpansionZipFile == null) {
                throw new ExtensionFileException("Expansion zip file variable is not initialized");
            }
            assetFileDescriptor = apkExpansionZipFile.getAssetFileDescriptor(fileName);
        }

        if (assetFileDescriptor == null) {
            throw new ExtensionFileException("Assert file descriptor for file: " + fileName +
                    " not found");
        }

        try {
            mMediaPlayer.setDataSource(assetFileDescriptor.getFileDescriptor(),
                    assetFileDescriptor.getStartOffset(),
                    assetFileDescriptor.getLength());
        } catch (IllegalArgumentException e) {
            throw new MediaPlayerException(e);
        } catch (IllegalStateException e) {
            throw new MediaPlayerException(e);
        } catch (IOException e) {
            throw new MediaPlayerException(e);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
    }

    @Override
    public void onSDLServiceConnected(SDLServiceBinder service) {
        Logger.i("SDLService connected " + service);

        mBoundSDLService = service.getService();
    }

    @Override
    public void onSDLServiceDisconnected() {
        Logger.i("SDLService disconnected");

        mBoundSDLService = null;
    }
}