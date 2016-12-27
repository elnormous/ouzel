// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity
{
    private SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        OuzelLibJNIWrapper.onCreated(getAssets());

        surfaceView = new SurfaceView(this, 8, 8, 8, 8, 0, 0);
        setContentView(surfaceView);
    }

    @Override
    protected void onPause()
    {
        super.onPause();

        if (surfaceView != null)
        {
            surfaceView.onPause();
        }
    }

    @Override
    protected void onResume()
    {
        super.onResume();

        if (surfaceView != null)
        {
            surfaceView.onResume();
        }
    }
}
