package com.warnengine.imperium;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class RendererWrapper implements GLSurfaceView.Renderer {
    public static native void on_surface_created();

    public static native void on_surface_changed();

    public static native void on_draw_frame();

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        on_surface_created();
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        on_surface_changed();
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        on_draw_frame();
    }
}

public class MainActivity extends Activity {
    public GLSurfaceView glSurfaceView;
    public boolean rendererSet;

    public static native void on_create(AssetManager assetManager);

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ActivityManager activityManager
                = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();

        on_create(getAssets());

        final boolean supportsEs2 =
                configurationInfo.reqGlEsVersion >= 0x30000;

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        if (supportsEs2) {
            glSurfaceView = new GLSurfaceView(this);

            glSurfaceView.setEGLContextClientVersion(3);
            glSurfaceView.setRenderer(new RendererWrapper());
            rendererSet = true;
            setContentView(glSurfaceView);
        } else {
            // Should never be seen in production, since the manifest filters
            // unsupported devices.
            Toast.makeText(this, "This device does not support OpenGL ES 3.0.",
                    Toast.LENGTH_LONG).show();
        }

        Log.println(Log.INFO, "native-lib", "Salut les petits gars");
        try {
            for (String assets : getAssets().list("/")) {
                Log.println(Log.INFO, "native-lib", assets);
            }
            for (String assets : getAssets().list("assets/")) {
                Log.println(Log.INFO, "native-lib", "Assets!:" + assets);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();

        if (rendererSet) {
            glSurfaceView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (rendererSet) {
            glSurfaceView.onResume();
        }
    }
}
