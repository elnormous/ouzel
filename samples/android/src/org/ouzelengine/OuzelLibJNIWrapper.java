// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

package org.ouzelengine;

import android.content.res.AssetManager;
import android.view.Surface;

public class OuzelLibJNIWrapper
{
    static
    {
        System.loadLibrary("samples");
    }

    public static native void onCreate(MainActivity mainActivity, AssetManager assetManager);
    public static native void setSurface(Surface surface);
    public static native void onSurfaceChanged(Surface surface, int width, int height);

    public static native void onStart();
    public static native void onPause();
    public static native void onResume();

    public static native void handleActionDown(int pointerId, float x, float y);
    public static native void handleActionMove(int pointerId, float x, float y);
    public static native void handleActionUp(int pointerId, float x, float y);
    public static native void handleActionCancel(int pointerId, float x, float y);
}
