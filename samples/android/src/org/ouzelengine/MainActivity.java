// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

package org.ouzelengine;

import android.app.Activity;
import android.os.Bundle;
import android.view.SurfaceHolder;

public class MainActivity extends Activity implements SurfaceHolder.Callback
{
    private View surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        OuzelLibJNIWrapper.setMainActivity(this);
        OuzelLibJNIWrapper.setAssetManager(getAssets());

        surfaceView = new View(this);
        surfaceView.getHolder().addCallback(this);
        setContentView(surfaceView);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        OuzelLibJNIWrapper.onSurfaceChanged(holder.getSurface(), width, height);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        OuzelLibJNIWrapper.setSurface(holder.getSurface());
        OuzelLibJNIWrapper.onStart();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        OuzelLibJNIWrapper.setSurface(null);
    }

    @Override
    protected void onStart()
    {
        super.onStart();
    }

    @Override
    protected void onPause()
    {
        super.onPause();

        OuzelLibJNIWrapper.onPause();
    }

    @Override
    protected void onResume()
    {
        super.onResume();

        OuzelLibJNIWrapper.onResume();
    }
}
