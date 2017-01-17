// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends Activity
{
    public static final String TAG = "Ouzel";
    private SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        OuzelLibJNIWrapper.onCreated(this, getAssets());
    }

    public void createSurface(int r, int g, int b, int a, int depth, int stencil, int sampleBuffers, int samples)
    {
        surfaceView = new SurfaceView(this, r, g, b, a, depth, stencil, sampleBuffers, samples);
        setContentView(surfaceView);
    }

    public void openURL(String url)
    {
        try
        {
            Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
            startActivity(browserIntent);
        }
        catch (Exception e)
        {
            Log.e(TAG, "Failed to open URL, error " + e.getMessage());
        }
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
