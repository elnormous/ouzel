// Ouzel by Elviss Strazdins

package org.ouzel;

import android.app.Activity;
import android.content.res.Configuration;
import android.os.Bundle;

public class MainActivity extends Activity
{
    @Override public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        OuzelLibJNIWrapper.onCreate(this);
        setContentView(new View(this));
    }

    @Override public void onStart()
    {
        super.onStart();
        OuzelLibJNIWrapper.onStart();
    }

    @Override public void onPause()
    {
        super.onPause();
        OuzelLibJNIWrapper.onPause();
    }

    @Override public void onResume()
    {
        super.onResume();
        OuzelLibJNIWrapper.onResume();
    }

    @Override public void onConfigurationChanged(Configuration newConfig)
    {
        super.onConfigurationChanged(newConfig);
        OuzelLibJNIWrapper.onConfigurationChanged(newConfig);
    }

    @Override public void onLowMemory()
    {
        super.onLowMemory();
        OuzelLibJNIWrapper.onLowMemory();
    }
}
