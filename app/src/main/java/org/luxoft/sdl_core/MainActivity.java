package org.luxoft.sdl_core;

import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final Button start_sdl = findViewById(R.id.start_sdl_button);
        final Button stop_sdl = findViewById(R.id.stop_sdl_button);

        start_sdl.setEnabled(true);
        stop_sdl.setEnabled(false);

        start_sdl.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (startSDL()) {
                    start_sdl.setEnabled(false);
                    stop_sdl.setEnabled(true);
                }
            }
        });

        stop_sdl.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                start_sdl.setEnabled(true);
                stop_sdl.setEnabled(false);
            }
        });
    }

    private boolean startSDL() {
        try {
            //System.loadLibrary("emhashmap");
            //System.loadLibrary("bson");
            System.loadLibrary("smartDeviceLinkCore");
        } catch (UnsatisfiedLinkError e) {
            showToastMessage("Failed to load the library: " + e.getMessage());
            return false;
        }

        showToastMessage("SDL library has been successfully loaded");
        return true;
    }

    private void showToastMessage(String message) {
        Context context = getApplicationContext();
        int duration = Toast.LENGTH_LONG;

        Toast toast = Toast.makeText(context, message, duration);
        toast.show();
    }

}
