package com.ford.avarsdl.service;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.IBinder;

import com.ford.avarsdl.views.SafeToast;
import com.ford.avarsdl.notifications.NotificationCommand;
import com.ford.avarsdl.notifications.NotificationCommandImpl;
import com.ford.avarsdl.responses.ResponseCommand;
import com.ford.avarsdl.util.Const;
import com.ford.avarsdl.util.Logger;
import com.ford.avarsdl.util.RPCConst;
import com.ford.syncV4.exception.SyncException;
import com.ford.syncV4.exception.SyncExceptionCause;
import com.ford.syncV4.proxy.SyncProxyALM;
import com.ford.syncV4.proxy.constants.Names;
import com.ford.syncV4.proxy.interfaces.IProxyListenerALM;
import com.ford.syncV4.proxy.rpc.AddCommandResponse;
import com.ford.syncV4.proxy.rpc.AddSubMenuResponse;
import com.ford.syncV4.proxy.rpc.AlertManeuverResponse;
import com.ford.syncV4.proxy.rpc.AlertResponse;
import com.ford.syncV4.proxy.rpc.CancelAccessResponse;
import com.ford.syncV4.proxy.rpc.ChangeRegistrationResponse;
import com.ford.syncV4.proxy.rpc.CreateInteractionChoiceSetResponse;
import com.ford.syncV4.proxy.rpc.DeleteCommandResponse;
import com.ford.syncV4.proxy.rpc.DeleteFileResponse;
import com.ford.syncV4.proxy.rpc.DeleteInteractionChoiceSetResponse;
import com.ford.syncV4.proxy.rpc.DeleteSubMenuResponse;
import com.ford.syncV4.proxy.rpc.EncodedSyncPDataResponse;
import com.ford.syncV4.proxy.rpc.EndAudioPassThruResponse;
import com.ford.syncV4.proxy.rpc.GenericResponse;
import com.ford.syncV4.proxy.rpc.GetDTCsResponse;
import com.ford.syncV4.proxy.rpc.GetVehicleDataResponse;
import com.ford.syncV4.proxy.rpc.GrantAccessResponse;
import com.ford.syncV4.proxy.rpc.ListFilesResponse;
import com.ford.syncV4.proxy.rpc.OnAudioPassThru;
import com.ford.syncV4.proxy.rpc.OnButtonEvent;
import com.ford.syncV4.proxy.rpc.OnButtonPress;
import com.ford.syncV4.proxy.rpc.OnCommand;
import com.ford.syncV4.proxy.rpc.OnControlChanged;
import com.ford.syncV4.proxy.rpc.OnDriverDistraction;
import com.ford.syncV4.proxy.rpc.OnEncodedSyncPData;
import com.ford.syncV4.proxy.rpc.OnHMIStatus;
import com.ford.syncV4.proxy.rpc.OnKeyboardInput;
import com.ford.syncV4.proxy.rpc.OnLanguageChange;
import com.ford.syncV4.proxy.rpc.OnPermissionsChange;
import com.ford.syncV4.proxy.rpc.OnPresetsChanged;
import com.ford.syncV4.proxy.rpc.OnRadioDetails;
import com.ford.syncV4.proxy.rpc.OnSyncPData;
import com.ford.syncV4.proxy.rpc.OnTBTClientState;
import com.ford.syncV4.proxy.rpc.OnTouchEvent;
import com.ford.syncV4.proxy.rpc.OnVehicleData;
import com.ford.syncV4.proxy.rpc.PerformAudioPassThruResponse;
import com.ford.syncV4.proxy.rpc.PerformInteractionResponse;
import com.ford.syncV4.proxy.rpc.PutFileResponse;
import com.ford.syncV4.proxy.rpc.ReadDIDResponse;
import com.ford.syncV4.proxy.rpc.RegisterAppInterface;
import com.ford.syncV4.proxy.rpc.ResetGlobalPropertiesResponse;
import com.ford.syncV4.proxy.rpc.ScrollableMessageResponse;
import com.ford.syncV4.proxy.rpc.SetAppIconResponse;
import com.ford.syncV4.proxy.rpc.SetDisplayLayoutResponse;
import com.ford.syncV4.proxy.rpc.SetGlobalPropertiesResponse;
import com.ford.syncV4.proxy.rpc.SetMediaClockTimerResponse;
import com.ford.syncV4.proxy.rpc.ShowConstantTBTResponse;
import com.ford.syncV4.proxy.rpc.ShowResponse;
import com.ford.syncV4.proxy.rpc.SliderResponse;
import com.ford.syncV4.proxy.rpc.SpeakResponse;
import com.ford.syncV4.proxy.rpc.SubscribeButtonResponse;
import com.ford.syncV4.proxy.rpc.SubscribeVehicleDataResponse;
import com.ford.syncV4.proxy.rpc.SyncMsgVersion;
import com.ford.syncV4.proxy.rpc.SyncPDataResponse;
import com.ford.syncV4.proxy.rpc.UnsubscribeButtonResponse;
import com.ford.syncV4.proxy.rpc.UnsubscribeVehicleDataResponse;
import com.ford.syncV4.proxy.rpc.UpdateTurnListResponse;
import com.ford.syncV4.proxy.rpc.enums.AppHMIType;
import com.ford.syncV4.proxy.rpc.enums.ChangeReason;
import com.ford.syncV4.proxy.rpc.enums.Language;
import com.ford.syncV4.transport.TCPTransportConfig;

import org.json.JSONException;

import java.io.IOException;
import java.util.Hashtable;
import java.util.Vector;

/**
 * Service that is responsible for communicating with the SDL.
 *
 * It's a started service, and doesn't support binding.
 *
 * Created by enikolsky on 2013-10-29
 * Co-author: Yuriy Chernyshov
 */
public class SDLService extends Service implements IProxyListenerALM {

    private static final String APP_ID = "42";
    private static final String APP_NAME = "MFTGuide";
    private static SyncProxyALM mSyncProxy;

    private final IBinder mBinder = new SDLServiceBinder(this);
    private static final Hashtable<String, NotificationCommand> commandsHashTable
            = new Hashtable<String, NotificationCommand>();
    private boolean isMobileInFocus = false;

    public static SyncProxyALM getProxyInstance() {
        return mSyncProxy;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Logger.d(getClass().getSimpleName() + " onStartCommand " + intent + ", " + flags + ", " +
                startId);

        try {
            initializeCommandsHashTable();
        } catch (IOException e) {
            // TODO: Probably to stop Service here or to dispatch this error
            Logger.e(getClass().getSimpleName() + " can not init commands: " + e.getMessage());
        }
        startProxyIfNetworkConnected();
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        Logger.d(getClass().getSimpleName() + " onDestroy, hash: " + hashCode());

        stopProxy();
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        Logger.d(getClass().getSimpleName() + " onBind, hash: " + hashCode());
        return mBinder;
    }

    private void startProxyIfNetworkConnected() {
        SharedPreferences prefs = getSharedPreferences(Const.PREFS_NAME, MODE_PRIVATE);
        int transportType = prefs.getInt(Const.PREFS_KEY_TRANSPORT_TYPE,
                Const.PREFS_DEFAULT_TRANSPORT_TYPE);

        if (transportType == Const.KEY_BLUETOOTH) {
            Logger.d(getClass().getSimpleName() + " ProxyService. onStartCommand()." +
                    "Transport = Bluetooth.");
            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            if (bluetoothAdapter != null) {
                if (bluetoothAdapter.isEnabled()) {
                    startProxy();
                }
            }
        } else {
            //TODO: This code is commented out for simulator purposes
			/*
			Log.d(TAG, "ProxyService. onStartCommand(). Transport = WiFi.");
			if (hasWiFiConnection() == true) {
				Log.d(TAG, "ProxyService. onStartCommand(). WiFi enabled.");
				startProxy();
			} else {
				Log.w(TAG,
						"ProxyService. onStartCommand(). WiFi is not enabled.");
			}
			*/
            startProxy();
        }
    }

    private void startProxy() {
        Logger.d(getClass().getSimpleName() + " Starting proxy ...");

        SharedPreferences sharedPreferences = getSharedPreferences(Const.PREFS_NAME, MODE_PRIVATE);
        String ipAddressString = sharedPreferences.getString(Const.PREFS_KEY_IPADDR,
                Const.PREFS_DEFAULT_IPADDR);
        int tcpPortInt = sharedPreferences.getInt(Const.PREFS_KEY_TCPPORT,
                Const.PREFS_DEFAULT_TCPPORT);

        SyncMsgVersion syncMsgVersion = new SyncMsgVersion();
        syncMsgVersion.setMajorVersion(2);
        syncMsgVersion.setMinorVersion(2);

        Vector<AppHMIType> appHMIType = new Vector<AppHMIType>();
        appHMIType.add(AppHMIType.RADIO);

        int transportType = sharedPreferences.getInt(Const.PREFS_KEY_TRANSPORT_TYPE,
                Const.PREFS_DEFAULT_TRANSPORT_TYPE);

        try {
            if (transportType == Const.KEY_BLUETOOTH) {
                Logger.i(getClass().getSimpleName() + " Start Bluetooth Proxy");
                mSyncProxy = new SyncProxyALM(this, null, APP_NAME, null, null, true,
                        appHMIType, syncMsgVersion, Language.EN_US, Language.EN_US, APP_ID,
                        null, false, false, 2);
            } else {
                Logger.i(getClass().getSimpleName() + " Start WiFi Proxy");
                mSyncProxy = new SyncProxyALM(this, null, APP_NAME, null, null, true,
                        appHMIType, syncMsgVersion, Language.EN_US, Language.EN_US, APP_ID,
                        null, false, false, 2,
                        new TCPTransportConfig(tcpPortInt, ipAddressString));
            }
        } catch (SyncException e) {
            Logger.e(getClass().getSimpleName() + " Failed to start proxy", e);
            if (mSyncProxy == null) {
                stopSelf();
            }
        }
    }

    private void stopProxy() {
        Logger.d(getClass().getSimpleName() + " Stopping proxy ...");
        if (mSyncProxy != null) {
            try {
                mSyncProxy.dispose();
                Logger.d(getClass().getSimpleName() + " Proxy is stopped");
            } catch (SyncException e) {
                Logger.e(getClass().getSimpleName() + " Failed to stop proxy", e);
            }
            mSyncProxy = null;
        }
    }

    @Override
    public void onOnHMIStatus(OnHMIStatus notification) {
        final String msg = "HMI Status " + notification.getHmiLevel() + ", " +
                notification.getSystemContext() + ", first run " + notification.getFirstRun();
        SafeToast.showToastAnyThread(msg);
    }

    @Override
    public void onProxyClosed(String info, Exception e) {
        //final String msg = "Proxy Closed. Info: " + info;
        //SafeToast.showToastAnyThread(msg);
        Logger.i(getClass().getSimpleName() + " On Proxy closed");
        final SyncExceptionCause cause = ((SyncException) e).getSyncExceptionCause();
        if ((cause != SyncExceptionCause.SYNC_PROXY_CYCLED) &&
                (cause != SyncExceptionCause.BLUETOOTH_DISABLED) &&
                (cause != SyncExceptionCause.SYNC_REGISTRATION_ERROR)) {
            resetProxy();
        }
    }

    public void resetProxy() {
        try {
            mSyncProxy.resetProxy();
        } catch (SyncException e) {
            e.printStackTrace();
            //something goes wrong, & the proxy returns as null, stop the service.
            //do not want a running service with a null proxy
            if (mSyncProxy == null) {
                stopSelf();
            }
        }
    }

    @Override
    public void onError(String info, Exception e) {
        String msg = "Proxy Error: " + info;
        SafeToast.showToastAnyThread(msg);
        if (e instanceof SyncException) {
            if (((SyncException) e).getSyncExceptionCause() == SyncExceptionCause.SYNC_UNAVAILALBE) {

                if (isMobileInFocus) {
                    OnControlChanged onControlChanged = new OnControlChanged();
                    onControlChanged.setParameters(Names.reason, ChangeReason.DRIVER_FOCUS);
                    onOnControlChanged(onControlChanged);

                    isMobileInFocus = false;
                }
            }
        }
    }

    @Override
    public void onGenericResponse(GenericResponse response) {
        String msg = "Generic response " + response.getResultCode();
        SafeToast.showToastAnyThread(msg);
    }

    @Override
    public void onOnCommand(OnCommand notification) {

    }

    @Override
    public void onAddCommandResponse(AddCommandResponse response) {

    }

    @Override
    public void onAddSubMenuResponse(AddSubMenuResponse response) {

    }

    @Override
    public void onCreateInteractionChoiceSetResponse(CreateInteractionChoiceSetResponse response) {

    }

    @Override
    public void onAlertResponse(AlertResponse response) {

    }

    @Override
    public void onDeleteCommandResponse(DeleteCommandResponse response) {

    }

    @Override
    public void onDeleteInteractionChoiceSetResponse(DeleteInteractionChoiceSetResponse response) {

    }

    @Override
    public void onDeleteSubMenuResponse(DeleteSubMenuResponse response) {

    }

    @Override
    public void onEncodedSyncPDataResponse(EncodedSyncPDataResponse response) {

    }

    @Override
    public void onSyncPDataResponse(SyncPDataResponse response) {

    }

    @Override
    public void onPerformInteractionResponse(PerformInteractionResponse response) {

    }

    @Override
    public void onResetGlobalPropertiesResponse(ResetGlobalPropertiesResponse response) {

    }

    @Override
    public void onSetGlobalPropertiesResponse(SetGlobalPropertiesResponse response) {

    }

    @Override
    public void onSetMediaClockTimerResponse(SetMediaClockTimerResponse response) {

    }

    @Override
    public void onShowResponse(ShowResponse response) {

    }

    @Override
    public void onSpeakResponse(SpeakResponse response) {

    }

    @Override
    public void onOnButtonEvent(OnButtonEvent notification) {

    }

    @Override
    public void onOnButtonPress(OnButtonPress notification) {

    }

    @Override
    public void onSubscribeButtonResponse(SubscribeButtonResponse response) {

    }

    @Override
    public void onUnsubscribeButtonResponse(UnsubscribeButtonResponse response) {

    }

    @Override
    public void onOnPermissionsChange(OnPermissionsChange notification) {

    }

    @Override
    public void onSubscribeVehicleDataResponse(SubscribeVehicleDataResponse response) {

    }

    @Override
    public void onUnsubscribeVehicleDataResponse(UnsubscribeVehicleDataResponse response) {

    }

    @Override
    public void onGetVehicleDataResponse(GetVehicleDataResponse response) {

    }

    @Override
    public void onReadDIDResponse(ReadDIDResponse response) {

    }

    @Override
    public void onGetDTCsResponse(GetDTCsResponse response) {

    }

    @Override
    public void onOnVehicleData(OnVehicleData notification) {

    }

    @Override
    public void onPerformAudioPassThruResponse(PerformAudioPassThruResponse response) {

    }

    @Override
    public void onEndAudioPassThruResponse(EndAudioPassThruResponse response) {

    }

    @Override
    public void onOnAudioPassThru(OnAudioPassThru notification) {

    }

    @Override
    public void onPutFileResponse(PutFileResponse response) {

    }

    @Override
    public void onDeleteFileResponse(DeleteFileResponse response) {

    }

    @Override
    public void onListFilesResponse(ListFilesResponse response) {

    }

    @Override
    public void onSetAppIconResponse(SetAppIconResponse response) {

    }

    @Override
    public void onScrollableMessageResponse(ScrollableMessageResponse response) {

    }

    @Override
    public void onChangeRegistrationResponse(ChangeRegistrationResponse response) {

    }

    @Override
    public void onSetDisplayLayoutResponse(SetDisplayLayoutResponse response) {

    }

    @Override
    public void onOnLanguageChange(OnLanguageChange notification) {

    }

    @Override
    public void onSliderResponse(SliderResponse response) {

    }

    @Override
    public void onAlertManeuverResponse(AlertManeuverResponse response) {

    }

    @Override
    public void onShowConstantTBTResponse(ShowConstantTBTResponse response) {

    }

    @Override
    public void onUpdateTurnListResponse(UpdateTurnListResponse response) {

    }

    @Override
    public void onGiveControlResponse(GrantAccessResponse response) {
        ResponseCommand command;
        try {
            command = new com.ford.avarsdl.responses.GrantAccessResponse();
            byte serializeMethod = 2;
            command.execute(response.getCorrelationID(),
                    response.serializeJSON(serializeMethod).toString());

            isMobileInFocus = true;

        } catch (JSONException e) {
            Logger.e(getClass().getSimpleName() + " onGiveControlResponse JSONException " + e);
        } catch (IOException e) {
            Logger.e(getClass().getSimpleName() + " onGiveControlResponse IOException " + e);
        }
    }

    @Override
    public void onCancelAccessResponse(CancelAccessResponse response) {
        ResponseCommand command;
        try {
            command = new com.ford.avarsdl.responses.CancelAccessResponse();
            byte serializeMethod = 2;
            command.execute(response.getCorrelationID(),
                    response.serializeJSON(serializeMethod).toString());
        } catch (JSONException e) {
            Logger.e(getClass().getSimpleName() + " onCancelAccessResponse JSONException " + e);
        } catch (IOException e) {
            Logger.e(getClass().getSimpleName() + " onCancelAccessResponse IOException " + e);
        }

        isMobileInFocus = false;
    }

    @Override
    public void onOnControlChanged(OnControlChanged notification) {
        //SafeToast.showToastAnyThread("onControlChanged " + notification);
        Logger.d(getClass().getSimpleName() + " onControlChanged " + notification);
        NotificationCommand command = commandsHashTable.get(Names.OnControlChanged);
        if (command != null) {
            String method = RPCConst.CN_REVSDL + "." + Names.OnControlChanged;
            command.execute(method, notification);
        } else {
            Logger.w(getClass().getSimpleName() + " NotificationCommand NULL");
        }
    }

    @Override
    public void onMobileNaviStart() {

    }

    @Override
    public void onMobileNavAckReceived(int frameReceivedNumber) {

    }

    @Override
    public void onOnTouchEvent(OnTouchEvent notification) {

    }

    @Override
    public void onKeyboardInput(OnKeyboardInput msg) {

    }

    @Override
    public void onRegisterAppRequest(RegisterAppInterface msg) {

    }

    @Override
    public void onOnPresetsChanged(OnPresetsChanged notification) {
        NotificationCommand command = commandsHashTable.get(Names.OnPresetsChanged);
        if (command != null) {
            String method = RPCConst.CN_REVSDL + "." + Names.OnPresetsChanged;
            command.execute(method, notification);
        } else {
            Logger.w(getClass().getSimpleName() + " NotificationCommand NULL");
        }
    }

    @Override
    public void onOnRadioDetails(OnRadioDetails notification) {
        // TODO: Expand notification information here
        //final RadioStation radioStation = notification.getRadioStation();
        //String msg = "onRadioDetails";
        //if (radioStation == null) {
        //    msg += " Radio Station invalid";
        //    //SafeToast.showToastAnyThread(msg);
        //    return;
        //}
        //msg += " frequency: " +
        //        notification.getRadioStation().getFrequency() + "." +
        //        notification.getRadioStation().getFraction();
        //SafeToast.showToastAnyThread(msg);

        NotificationCommand command = commandsHashTable.get(Names.OnRadioDetails);
        if (command != null) {
            String method = RPCConst.CN_REVSDL + "." + Names.OnRadioDetails;
            command.execute(method, notification);
        } else {
            Logger.w(getClass().getSimpleName() + " NotificationCommand NULL");
        }
    }

    @Override
    public void onOnDriverDistraction(OnDriverDistraction notification) {

    }

    @Override
    public void onOnEncodedSyncPData(OnEncodedSyncPData notification) {

    }

    @Override
    public void onOnSyncPData(OnSyncPData notification) {

    }

    @Override
    public void onOnTBTClientState(OnTBTClientState notification) {

    }

    private void initializeCommandsHashTable() throws IOException {
        // TODO: Probably in the future version there will be differences between notification
        // objects, but up to now they contain general information structure

        NotificationCommand onRadioDetailsNotification = new NotificationCommandImpl();
        commandsHashTable.put(Names.OnRadioDetails, onRadioDetailsNotification);

        NotificationCommand onControlChangedNotification = new NotificationCommandImpl();
        commandsHashTable.put(Names.OnControlChanged, onControlChangedNotification);

        NotificationCommand onPresetsChangedNotification = new NotificationCommandImpl();
        commandsHashTable.put(Names.OnPresetsChanged, onPresetsChangedNotification);
    }
}