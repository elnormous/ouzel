// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

import android.content.res.AssetManager;
import android.view.Surface;

public class OuzelLibJNIWrapper
{
    static
    {
        System.loadLibrary("samples");
    }

    public static native void setMainActivity(MainActivity mainActivity);
    public static native void setAssetManager(AssetManager assetManager);
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
