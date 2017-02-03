// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;

public class MainActivity extends Activity
{
    public static final String TAG = "Ouzel";
    private View surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        surfaceView = new View(this);
        setContentView(surfaceView);

        OuzelLibJNIWrapper.setMainActivity(this);
        OuzelLibJNIWrapper.setAssetManager(getAssets());
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

    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        OuzelLibJNIWrapper.setSurface(holder.getSurface(), width, height);
    }

    @Override
    protected void onStart()
    {
        super.onStart();

        OuzelLibJNIWrapper.onStart();
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
