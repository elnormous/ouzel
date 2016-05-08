// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

public class OuzelLibJNIWrapper
{
	static
	{
		System.loadLibrary("sprite");
	}

	public static native void onSurfaceCreated();

	public static native void onSurfaceChanged(int width, int height);

	public static native void onDrawFrame();
}
