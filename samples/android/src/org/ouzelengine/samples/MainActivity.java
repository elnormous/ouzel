// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

package org.ouzelengine.samples;

import org.ouzelengine.OuzelLibJNIWrapper;
import org.ouzelengine.View;
import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity
{
    @Override protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        OuzelLibJNIWrapper.onCreate(this);
        setContentView(new View(this));
    }

    @Override protected void onStart()
    {
        super.onStart();
        OuzelLibJNIWrapper.onStart();
    }

    @Override protected void onPause()
    {
        super.onPause();
        OuzelLibJNIWrapper.onPause();
    }

    @Override protected void onResume()
    {
        super.onResume();
        OuzelLibJNIWrapper.onResume();
    }

    @Override public void onLowMemory()
    {
        super.onLowMemory();
        OuzelLibJNIWrapper.onLowMemory();
    }
}
