package org.luxoft.sdl_core;

import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

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

        sdl_external_dir_folder_path = Environment.getExternalStorageDirectory().toString().concat("/SDL");
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
