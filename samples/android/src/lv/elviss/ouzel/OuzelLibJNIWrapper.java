// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

public class OuzelLibJNIWrapper
{
	static
	{
		System.loadLibrary("samples");
	}

	public static native void onSurfaceCreated();

	public static native void onSurfaceChanged(int width, int height);

	public static native void onDrawFrame();
}
