// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

package org.ouzelengine;

import android.app.Activity;
import android.content.res.Configuration;
import android.view.MotionEvent;
import android.view.Surface;

public class OuzelLibJNIWrapper
{
    static
    {
        System.loadLibrary("samples");
    }

    public static native void onCreate(Activity mainActivity);
    public static native void onSurfaceCreated(Surface surface);
    public static native void onSurfaceDestroyed();
    public static native void onSurfaceChanged(Surface surface, int width, int height);

    public static native void onStart();
    public static native void onPause();
    public static native void onResume();
    public static native void onConfigurationChanged(Configuration newConfig);
    public static native void onLowMemory();

    public static native void onBackPressed();
    public static native void onKeyDown(int keyCode);
    public static native void onKeyUp(int keyCode);

    public static native boolean onTouchEvent(MotionEvent event);
}
