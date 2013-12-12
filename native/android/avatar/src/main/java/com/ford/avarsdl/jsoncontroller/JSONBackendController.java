package com.ford.avarsdl.jsoncontroller;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Rect;
import android.os.Build;
import android.os.Message;
import android.util.DisplayMetrics;
import android.view.Window;

import com.ford.avarsdl.util.AppUtils;
import com.ford.avarsdl.views.AvatarActivity;
import com.ford.avarsdl.views.EulaActivity;
import com.ford.avarsdl.views.SafeToast;
import com.ford.avarsdl.jsonparser.EBEMethods;
import com.ford.avarsdl.util.Const;
import com.ford.avarsdl.util.Logger;
import com.ford.avarsdl.util.RPCConst;
import com.ford.avarsdl.util.Utils;

import java.io.IOException;

public class JSONBackendController extends JSONController {

    public JSONBackendController(AvatarActivity activity) throws IOException {
        super(RPCConst.CN_BACKEND);
        mActivity = activity;
    }

    public JSONBackendController(ITCPClient client) throws IOException {
        super(RPCConst.CN_BACKEND, client);
    }

	public void sendFullScreenRequest(Boolean value) {
		String method = RPCConst.CN_BACKEND_CLIENT + "." + EBEMethods.onFullScreenChanged.toString();
		mJSONParser.putEmptyJSONObject();
		mJSONParser.putBooleanValue("isFullScreen", value);
		sendRequest(method, mJSONParser.getJSONObject());
	}

	public void sendHasMapsNotification(Boolean value) {
		String method = RPCConst.CN_BACKEND + "." + EBEMethods.onHasMapsChanged.toString();
		mJSONParser.putEmptyJSONObject();
		mJSONParser.putBooleanValue("hasMaps", value);
		sendNotification(method, mJSONParser.getJSONObject());
	}

	public void sendVehicleNotification(String value) {
		String method = RPCConst.CN_BACKEND + "." + EBEMethods.onVehicleChanged.toString();
		mJSONParser.putEmptyJSONObject();
		mJSONParser.putStringValue("vehicle", value);
		sendNotification(method, mJSONParser.getJSONObject());
	}

	// =======================================================
	// private and protected section
	// =======================================================
	// private String mResponse;
	private AvatarActivity mActivity;

	protected void processRequest(String request) {
		String result = processNotification(request);
		mJSONParser.putJSONObject(request);
		sendResponse(mJSONParser.getId(), result);
	}

	protected String processNotification(String notification) {
		mJSONParser.putJSONObject(notification);
		String method = mJSONParser.getMethod();
		method = method.substring(method.indexOf('.') + 1, method.length());
		String out = null;
		switch (EBEMethods.valueOf(method)) {
		case isFirstStart:
			out = isFirstStart();
			break;
		case isFullScreen:
			out = isFullScreen();
			break;
		case getWindowSize:
			out = getWindowSize();
			break;
		case getOSInfo:
			out = getOSInfo();
			break;
		case getWindowDensity:
			out = getWindowDensity();
			break;
		case hasMaps:
			out = hasMaps();
			break;
		case getVehicleModel:
			out = getVehicleModel();
			break;
		case setHasMaps:
			out = setHasMaps();
			break;
		case setVehicleModel:
			out = setVehicleModel();
			break;
		case logToOS:
			mJSONParser.putJSONObject(mJSONParser.getParams());
			Logger.i(getClass().getSimpleName() + " " + mJSONParser.getStringParam("message"));
			out = null;
			break;
		case onAppLoaded:
			Logger.i(getClass().getSimpleName() +  " onAppLoaded");
			Message msg = mActivity.getMainHandler().obtainMessage(Const.WEBVIEW_SHOW, null);
			mActivity.getMainHandler().sendMessage(msg);
			out = null;
			break;
		case sendSupportEmail:
			out = sendSupportEmail();
			break;
		case openEULA:
			out = openEula();
			break;
		default:
			mJSONParser.putEmptyJSONRPCObject();
			mJSONParser.putStringValue(RPCConst.TAG_ERROR,
                    "Backend does not support function : " + method);
			out = mJSONParser.getJSONObject();
			break;
		}
		return out;
	}

	protected void processResponse(String response) {
		if (!processRegistrationResponse(response)) {
			mJSONParser.putJSONObject(response);
			mRequestResponse = mJSONParser.getResult();// EBEMethods.onFullScreenChanged
			String method = mWaitResponseQueue.get(mJSONParser.getId());
			// get method name without component name
			if (method != null) {
				// get method name without component name
				method = method.substring(method.indexOf(".") + 1,
						method.length());
				switch (EBEMethods.valueOf(method)) {
				case onFullScreenChanged:
					if (mActivity.getVideoPlayed()) {
						// set new top, left and scale for video resizing
						mJSONParser.putJSONObject(mRequestResponse);
						int top = mJSONParser.getIntParam("top");
						int left = mJSONParser.getIntParam("left");
						double scale = mJSONParser.getDoubleParam("scale");
						// send msg to handler, because view should be modified
						// in it's thread
						Object obj[] = { scale, left, top };
						Message msg = mActivity.getMainHandler().obtainMessage(
								Const.VIDEO_PLAY_AFTER_SCALE, obj);
						mActivity.getMainHandler().sendMessage(msg);
					}
					;
					break;
				default:
					break;
				}
			}// if(method == null)
		}
	}

	protected void subscribeToProperties() {
		subscribeTo("BackendClient.onAppLoaded");
	}

	/**
	 * Look on previous code version from shared preferences and if it is lower
	 * than current one then it is considered that this is the first launch
	 * 
	 * @return
	 */
	private String isFirstStart() {
		boolean firstStart = mActivity.isFirstStart();
		if (firstStart) {
			firstStart = true;
			// set new code version
			SharedPreferences prefs = mActivity.getSharedPreferences(Const.SHPREF_FIRST_LAUNCH, 0);
			SharedPreferences.Editor editor = prefs.edit();
			if (editor != null) {
				int currentCodeVersion = Utils.getAppVersionCode(mActivity);
				editor.putInt(Const.SHPREF_PREVIOUS_CODE_VERSION, currentCodeVersion);
			}
			editor.commit();
			// set redownload counter to 0
			SharedPreferences downloaderPrefs = mActivity.getSharedPreferences(
					Const.SHPREF_DOWNLOADER_PREFS, 0);
			editor = downloaderPrefs.edit();
			if (editor != null) {
				editor.putInt(Const.SHPREF_REDOWNLOAD_COUNTER, 0);
			}
			editor.commit();
		}

		mJSONParser.putEmptyJSONObject();
		mJSONParser.putBooleanValue("isFirstStart", firstStart);
		return mJSONParser.getJSONObject();
	}

	private String getWindowSize() {
		mJSONParser.putEmptyJSONObject();
		mJSONParser.putIntValue("width", getWindowWidth());
		mJSONParser.putIntValue("height", getWindowHeight());
		return mJSONParser.getJSONObject();
	}

	private Integer getWindowHeight() {
		Integer height;
		if ((Build.VERSION.SDK_INT > 10)/* && (Build.VERSION.SDK_INT < 14) */) {
			// return height without tabs bar
			Rect rectgle = new Rect();
			Window window = mActivity.getWindow();
			window.getDecorView().getWindowVisibleDisplayFrame(rectgle);
			height = rectgle.bottom; // height = - panel height
		} else {
			DisplayMetrics dm = new DisplayMetrics();
			mActivity.getWindowManager().getDefaultDisplay().getMetrics(dm);
			if (dm.heightPixels > dm.widthPixels) {
				height = dm.widthPixels;
			} else {
				height = dm.heightPixels;
			}
		}
		Logger.i(getClass().getSimpleName() +  " GetWindowHeight = " + height);
		return height;
	}

	private Integer getWindowWidth() {
		Integer width;
		DisplayMetrics dm = new DisplayMetrics();
		mActivity.getWindowManager().getDefaultDisplay().getMetrics(dm);
		if (dm.heightPixels > dm.widthPixels) {
			width = dm.heightPixels;
		} else {
			width = dm.widthPixels;
		}
		Logger.i(getClass().getSimpleName() +  " GetWindowWidth = " + width);
		return width;
	}

	private String getWindowDensity() {
		DisplayMetrics dm = new DisplayMetrics();
		mActivity.getWindowManager().getDefaultDisplay().getMetrics(dm);
		Float dens = dm.density;
		Logger.i(getClass().getSimpleName() +  " GetWindowDpi = " + dens);
		mJSONParser.putEmptyJSONObject();
		mJSONParser.putDoubleValue("windowDensity", dens.doubleValue());
		return mJSONParser.getJSONObject();
	}

	private String isFullScreen() {
		boolean fullScreen = mActivity.getFullscreenStatus();
		Logger.i(getClass().getSimpleName() +  " Full screen = " + fullScreen);
		mJSONParser.putEmptyJSONObject();
		mJSONParser.putBooleanValue("isFullScreen", fullScreen);
		return mJSONParser.getJSONObject();
	}

	private String hasMaps() {
		boolean hasMaps = mActivity.getMapsStatus();
		Logger.i(getClass().getSimpleName() + " hasMaps = " + hasMaps);
		mJSONParser.putEmptyJSONObject();
		mJSONParser.putBooleanValue("hasMaps", hasMaps);
		return mJSONParser.getJSONObject();
	}

	private String getVehicleModel() {
		String vehicle = mActivity.getVehicleStatus();
		Logger.i(getClass().getSimpleName() + " hasMaps = " + vehicle);
		mJSONParser.putEmptyJSONObject();
		mJSONParser.putStringValue("vehicle", vehicle);
		return mJSONParser.getJSONObject();
	}

	private String setHasMaps() {
		mJSONParser.putJSONObject(mJSONParser.getParams());
		boolean param = mJSONParser.getBooleanParam("hasMaps");
		if (mActivity.setMapsStatus(param)) {
			mJSONParser.putEmptyJSONObject();
			mJSONParser.putStringValue("result", "OK");
			return mJSONParser.getJSONObject();
		} else {
			mJSONParser.putEmptyJSONObject();
			mJSONParser.putStringValue("result",
                    "!!! ERROR in navigation setting");
			return mJSONParser.getJSONObject();
		}
	}

	private String setVehicleModel() {
		mJSONParser.putJSONObject(mJSONParser.getParams());
		String param = mJSONParser.getStringParam("vehicle");
		if (mActivity.setVehicleStatus(param)) {
			mJSONParser.putEmptyJSONObject();
			mJSONParser.putStringValue("result", "OK");
			return mJSONParser.getJSONObject();
		} else {
			mJSONParser.putEmptyJSONObject();
			mJSONParser
					.putStringValue("result", "!!! ERROR in vehicle setting");
			return mJSONParser.getJSONObject();
		}
	}

	private String getOSInfo() {
		mJSONParser.putEmptyJSONObject();
		mJSONParser.putStringValue("OS", "Android");
		mJSONParser.putStringValue("OSVersion", AppUtils.getOSVersion());
		mJSONParser.putBooleanValue("isNative", true);
		return mJSONParser.getJSONObject();
	}

	private String openEula() {
		Intent intent = new Intent(mActivity, EulaActivity.class);
		intent.putExtra("firstStart", mActivity.isFirstStart());
		mActivity.startActivity(intent);
		mJSONParser.putEmptyJSONObject();
		mJSONParser.putStringValue("result", "OK");
		return mJSONParser.getJSONObject();
	}

	private String sendSupportEmail() {
		mJSONParser.putEmptyJSONObject();
		Intent i = new Intent(Intent.ACTION_SEND);
		i.setType("message/rfc822");
		i.putExtra(Intent.EXTRA_EMAIL, new String[] { Const.SUPPORT_EMAIL });
		i.putExtra(Intent.EXTRA_SUBJECT, "subject of email");
		i.putExtra(Intent.EXTRA_TEXT, "Sent from Android OS v." + AppUtils.getOSVersion());
		try {
			mActivity.startActivity(Intent.createChooser(i, "Send mail..."));
			mJSONParser.putStringValue("result", "OK");
		} catch (android.content.ActivityNotFoundException ex) {
            SafeToast.showToastAnyThread("There are no email clients installed.");
			mJSONParser.putStringValue("result", "Error!!! There are no email clients installed.");
		}
		return mJSONParser.getJSONObject();
	}
}