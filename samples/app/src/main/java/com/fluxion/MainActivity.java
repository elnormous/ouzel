package com.fluxion;

import android.app.Activity;
import android.content.res.Configuration;
import android.os.Bundle;

public class MainActivity extends Activity
{
    @Override public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        FluxionLibJNIWrapper.onCreate(this);
        setContentView(new View(this));
    }

    @Override public void onStart()
    {
        super.onStart();
        FluxionLibJNIWrapper.onStart();
    }

    @Override public void onPause()
    {
        super.onPause();
        FluxionLibJNIWrapper.onPause();
    }

    @Override public void onResume()
    {
        super.onResume();
        FluxionLibJNIWrapper.onResume();
    }

    @Override public void onConfigurationChanged(Configuration newConfig)
    {
        super.onConfigurationChanged(newConfig);
        FluxionLibJNIWrapper.onConfigurationChanged(newConfig);
    }

    @Override public void onLowMemory()
    {
        super.onLowMemory();
        FluxionLibJNIWrapper.onLowMemory();
    }
}
