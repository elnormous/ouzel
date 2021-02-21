// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

package org.ouzel;

import android.app.Activity;
import android.content.res.Configuration;
import android.view.MotionEvent;
import android.view.Surface;

public class OuzelLibJNIWrapper
{
    static
    {
        System.loadLibrary("main");
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

    public static native boolean onKeyDown(int keyCode);
    public static native boolean onKeyUp(int keyCode);

    public static native boolean onTouchEvent(MotionEvent event);
    public static native boolean onGenericMotionEvent(MotionEvent event);
}
