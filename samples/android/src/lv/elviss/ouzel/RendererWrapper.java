// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView.Renderer;

public class RendererWrapper implements Renderer
{
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		OuzelLibJNIWrapper.onSurfaceCreated();
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
		OuzelLibJNIWrapper.onSurfaceChanged(width, height);
	}

	@Override
	public void onDrawFrame(GL10 gl)
	{
		OuzelLibJNIWrapper.onDrawFrame();
	}
}
