package org.luxoft.sdl_core;

import android.Manifest;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.net.Uri;
import android.nfc.Tag;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import androidx.core.content.ContextCompat;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.documentfile.provider.DocumentFile;
import android.bluetooth.BluetoothAdapter;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;


public class MainActivity extends AppCompatActivity {

    static final String TAG = MainActivity.class.getSimpleName();

    private Thread sdl_thread_ = null;
    private boolean is_first_load_ = true;
    private Button start_sdl_button;
    private Button stop_sdl_button;
    private Button exit_button;

    public static String sdl_cache_folder_path;
    public static String sdl_external_dir_folder_path;
    private static final int ACCESS_LOCATION_REQUEST = 1;
    private static final int ACCESS_EXT_STORAGE_REQUEST = 2;

    private native static void StartSDL();
    private native static void StopSDL();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        start_sdl_button = findViewById(R.id.start_sdl_button);
        stop_sdl_button = findViewById(R.id.stop_sdl_button);
        exit_button = findViewById(R.id.exit_button);

        start_sdl_button.setEnabled(true);
        stop_sdl_button.setEnabled(false);

        start_sdl_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (startSDL()) {
                    start_sdl_button.setEnabled(false);
                    stop_sdl_button.setEnabled(true);
                }

                if (isBleSupported() && isBluetoothPermissionGranted()) {
                    startService(new Intent(MainActivity.this, BleCentralService.class));
                }
            }
        });

        stop_sdl_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                StopSDL();

                if (isBleSupported()) {
                    stopService(new Intent(MainActivity.this, BleCentralService.class));
                }
            }
        });

        exit_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
                System.exit(0);
            }
        });

        sdl_cache_folder_path = getDataDir().toString();
        TextView cache_folder_view = findViewById(R.id.cache_folder);
        cache_folder_view.setText(String.format("Cache folder: %s", sdl_cache_folder_path));

        if (isStorageMounted()) {
            if (isStoragePermissionGranted()) {
                updateExternalDirField(getExternalDirectory());
                runInitializeAssetsThread();
            } else {
                updateExternalDirField(getDefaultExternalDirectory()); // fallback path
                askForStoragePermissions();
            }
        }

        if (savedInstanceState == null) {
            initBT();
        }
    }

    private void updateExternalDirField(final String path) {
        sdl_external_dir_folder_path = path;
        TextView external_folder_view = findViewById(R.id.external_folder);
        external_folder_view.setText(String.format("External folder: %s", sdl_external_dir_folder_path));
    }

    private void runInitializeAssetsThread() {
        final ProgressDialog dialog = new ProgressDialog(MainActivity.this);
        dialog.setMessage("Initializing assets..");
        dialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        dialog.setCancelable(false);
        dialog.show();

        Thread thread = new Thread() {
            @Override
            public void run() {
                initializeAssets();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        dialog.dismiss();
                    }
                });
            }
        };

        thread.start();
    }

    private ArrayList<String> getAvailableAbi(AssetManager manager) throws IOException {
        ArrayList<String> output = new ArrayList<>();
        String[] available_archs = manager.list("");
        for (String available : available_archs) {
            String[] available_content = manager.list(available);
            for (String content : available_content) {
                if (content.contains(".so")) {
                    output.add(available);
                    break;
                }
            }
        }

        return output;
    }

    private String getPreferableAbi(ArrayList<String> supported, ArrayList<String> available) {
        for (String supported_abi : supported ) {
            if (available.contains(supported_abi)) {
                Log.d(TAG, "Supported ABI " + supported_abi + " assets are available. Use this ABI for initialization");
                return supported_abi;
            }

            Log.d(TAG, "ABI " + supported_abi + " is not available. Check the next supported");
        }

        // In case if no ABI supported
        if (!available.isEmpty()) {
            String first_available = available.get(0);
            Log.w(TAG, "No supported ABI found. Use first available " + first_available);
            return first_available;
        }

        Log.e(TAG, "No any available ABI found. Exiting");
        return null;
    }

    private void initializeAssets() {
        Log.d(TAG, "Initializing assets");

        try {
            AssetManager assetManager = getAssets();
            ArrayList<String> available_abi = getAvailableAbi(assetManager);
            ArrayList<String> supported_abi = new ArrayList<String>(Arrays.asList(Build.SUPPORTED_ABIS));
            String target_abi = getPreferableAbi(supported_abi, available_abi);
            if (target_abi == null) {
                return;
            }

            String target_folder = getFilesDir().toString();
            String[] assets = assetManager.list(target_abi);
            for (String asset : assets) {
                Log.d(TAG, "Found asset: " + asset);

                File target_asset_file = new File(target_folder + File.separator + asset);
                if (target_asset_file.exists()) {
                    Log.d(TAG, "Asset already initialized in " + target_asset_file);
                    continue;
                }

                Log.d(TAG, "Initializing asset: " + target_asset_file);

                InputStream in = assetManager.open(target_abi + File.separator + asset);
                DataOutputStream outw = new DataOutputStream(new FileOutputStream(
                    target_asset_file.getAbsolutePath()));

                final int max_buffer_size = 80000;
                byte[] buf = new byte[max_buffer_size];
                int len;
                while ((len = in.read(buf, 0, max_buffer_size)) > 0) {
                    outw.write(buf, 0, len);
                }

                in.close();
                outw.close();
            }

        } catch (IOException e) {
            Log.e(TAG, "Exception during assets initialization: " + e.toString());
        }
    }

    private String getWritableExternalDirectory() {
        // Create a temporary file to see whether a volume is really writeable.
        // It's important not to put it in the root directory which may have a
        // limit on the number of files.
        String directoryName = Environment.getExternalStorageDirectory().toString().concat("/SDL");

        File directory = new File(directoryName);
        if (!directory.isDirectory()) {
            if (!directory.mkdirs()) {
                return null;
            }
        }

        return directoryName;
    }

    private static boolean isStorageMounted() {
        String state = Environment.getExternalStorageState();
        return Environment.MEDIA_MOUNTED.equals(state);
    }

    private String getDefaultExternalDirectory() {
        return getDir("SDL", 0).getPath();
    }

    private String getExternalDirectory() {
        String path = getWritableExternalDirectory();

        if (path == null) {
            path = getDefaultExternalDirectory();
        }

        return path;
    }

    private void askForStoragePermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
          startActivityForResult(new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE), ACCESS_EXT_STORAGE_REQUEST);
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
          ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, ACCESS_EXT_STORAGE_REQUEST);
        }

        //permission is automatically granted on sdk < 23 upon installation, no need to ask
    }

    private void askForBluetoothPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, ACCESS_LOCATION_REQUEST);
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[] { Manifest.permission.ACCESS_COARSE_LOCATION }, ACCESS_LOCATION_REQUEST);
        }

        //permission is automatically granted on sdk < 23 upon installation, no need to ask
    }

    public boolean isStoragePermissionGranted() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            if (PackageManager.PERMISSION_GRANTED == checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
                Log.v(TAG,"Storage permission is granted");
                return true;
            } else {
                Log.v(TAG,"Storage permission is revoked");
                return false;
            }
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (PackageManager.PERMISSION_GRANTED == checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
                Log.v(TAG,"Storage permission is granted");
                return true;
            } else {
                Log.v(TAG,"Storage permission is revoked");
                return false;
            }
        } else { //permission is automatically granted on sdk < 23 upon installation
            Log.v(TAG,"Storage Permission is granted");
            return true;
        }
    }

    private boolean isBluetoothPermissionGranted() {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            if (getApplicationContext().checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                Log.v(TAG,"Bluetooth permission is revoked");
                return false;
            }
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (getApplicationContext().checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                Log.v(TAG,"Bluetooth permission is revoked");
                return false;
            }
        }

        Log.v(TAG,"Bluetooth permission is granted");
        return true;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent resultData) {
        super.onActivityResult(requestCode, resultCode, resultData);

        if (resultCode == RESULT_OK) {
            Uri treeUri = resultData.getData();
            DocumentFile pickedDir = DocumentFile.fromTreeUri(this, treeUri);
            grantUriPermission(getPackageName(), treeUri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
            getContentResolver().takePersistableUriPermission(treeUri, Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

            if (pickedDir != null) {
                  // TODO: Add support for a file operations for Anroid 10 on external storage
            }
        }

        runInitializeAssetsThread();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        switch (requestCode) {
            case ACCESS_LOCATION_REQUEST: {
                if (grantResults.length > 0) {
                    if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                        Log.i(TAG, "Bluetooth permissions are granted");
                    }
                }
                break;
            }
            case ACCESS_EXT_STORAGE_REQUEST: {
                if (grantResults.length > 0) {
                    if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                        Log.i(TAG, "External storage permissions are granted");
                        updateExternalDirField(getWritableExternalDirectory());
                    }
                }

                runInitializeAssetsThread();
                break;
            }

            default:
                super.onRequestPermissionsResult(requestCode, permissions, grantResults);
                break;
        }
    }

    private void onSdlStopped() {
        start_sdl_button.setEnabled(true);
        stop_sdl_button.setEnabled(false);
        showToastMessage("SDL has been stopped");
    }

    private boolean startSDL() {
        if (is_first_load_) {
            try {
                System.loadLibrary("c++_shared");
                System.loadLibrary("emhashmap");
                System.loadLibrary("bson");
                System.loadLibrary("boost_system");
                System.loadLibrary("boost_regex");
                System.loadLibrary("boost_thread");
                System.loadLibrary("boost_date_time");
                System.loadLibrary("boost_filesystem");
                System.loadLibrary("smartDeviceLinkCore");
                showToastMessage("SDL libraries has been successfully loaded");
                is_first_load_ = false;
            } catch (UnsatisfiedLinkError e) {
                showToastMessage("Failed to load the library: " + e.getMessage());
                return false;
            }
        }

        sdl_thread_ = new Thread() {
            @Override
            public void run() {
                StartSDL();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        onSdlStopped();
                    }
                });
            }
        };

        sdl_thread_.start();
        showToastMessage("SDL has been started");

        return true;
    }

    private boolean isBleSupported(){
        return BluetoothAdapter.getDefaultAdapter() != null &&
                getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE);
    }

    private void initBT() {
        if(!isBleSupported()){
            showToastMessage("BLE is NOT supported");
            return;
        }

        if (!isBluetoothPermissionGranted()) {
            askForBluetoothPermissions();
        }
    }

    private void showToastMessage(String message) {
        Context context = getApplicationContext();
        int duration = Toast.LENGTH_LONG;

        Toast toast = Toast.makeText(context, message, duration);
        toast.show();
    }

}
