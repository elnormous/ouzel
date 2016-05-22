// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

import android.content.res.AssetManager;

public class OuzelLibJNIWrapper
{
	static
	{
		System.loadLibrary("samples");
	}

    public static native void setAssetManager(AssetManager assetManager);

	public static native void onSurfaceCreated();

	public static native void onSurfaceChanged(int width, int height);

	public static native void onDrawFrame();
	
	public static native void handleActionDown(int pointerId, float x, float y);
	public static native void handleActionMove(int pointerId, float x, float y);
	public static native void handleActionUp(int pointerId, float x, float y);
	public static native void handleActionCancel(int pointerId, float x, float y);
}
