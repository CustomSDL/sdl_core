package org.luxoft.sdl_core;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.res.AssetManager;
import android.nfc.Tag;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    static final String TAG = MainActivity.class.getSimpleName();

    private Thread sdl_thread_ = null;
    private boolean is_first_load_ = true;
    private Button start_sdl_button;
    private Button stop_sdl_button;
    public static String sdl_cache_folder_path;
    public static String sdl_external_dir_folder_path;

    private native static void StartSDL();
    private native static void StopSDL();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        sdl_cache_folder_path = getCacheDir().toString();
        TextView cache_folder_view = findViewById(R.id.cache_folder);
        cache_folder_view.setText(String.format("Cache folder: %s", sdl_cache_folder_path));

        sdl_external_dir_folder_path = getFilesDir().toString();
        TextView external_folder_view = findViewById(R.id.external_folder);
        external_folder_view.setText(String.format("External folder: %s", sdl_external_dir_folder_path));

        start_sdl_button = findViewById(R.id.start_sdl_button);
        stop_sdl_button = findViewById(R.id.stop_sdl_button);

        start_sdl_button.setEnabled(true);
        stop_sdl_button.setEnabled(false);

        start_sdl_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (startSDL()) {
                    start_sdl_button.setEnabled(false);
                    stop_sdl_button.setEnabled(true);
                }
            }
        });

        stop_sdl_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                StopSDL();
            }
        });

        final ProgressDialog dialog = new ProgressDialog(MainActivity.this);
        dialog.setMessage("Initializing assets..");
        dialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        dialog.setCancelable(false);
        dialog.show();

        Thread thread = new Thread() {
            @Override
            public void run() {
                InitializeAssets();
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

    private void InitializeAssets() {
        String target_folder = getFilesDir().toString();
        String arch = System.getProperty("os.arch");

        Log.d(TAG, "Initializing assets for " + arch);

        try {
            AssetManager assetManager = getAssets();
            String[] assets = assetManager.list(arch);

            for (String asset : assets) {
                Log.d(TAG, "Found asset: " + asset);

                File target_asset_file = new File(target_folder + File.separator + asset);
                if (target_asset_file.exists()) {
                    Log.d(TAG, "Asset already initialized in " + target_asset_file);
                    continue;
                }

                Log.d(TAG, "Initializing asset: " + target_asset_file);

                InputStream in = assetManager.open(arch + File.separator + asset);
                DataOutputStream outw = new DataOutputStream(new FileOutputStream(
                    target_asset_file.getAbsolutePath()));

                final int max_buffer_size = 80000;
                byte[] buf = new byte[max_buffer_size];
                int len;
                while ((len = in.read(buf)) > 0) {
                    outw.write(buf, 0, len);
                }

                in.close();
                outw.close();
            }

        } catch (IOException e) {
            Log.e(TAG, "Exception during assets initialization: " + e.toString());
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

    private void showToastMessage(String message) {
        Context context = getApplicationContext();
        int duration = Toast.LENGTH_LONG;

        Toast toast = Toast.makeText(context, message, duration);
        toast.show();
    }

}
