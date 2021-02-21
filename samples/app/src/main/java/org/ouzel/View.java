// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

package org.ouzel;

import android.content.Context;
import android.view.InputDevice;
import android.view.InputEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.SparseArray;

public class View extends SurfaceView implements SurfaceHolder.Callback
{
    private final Handler messageHandler;

    private static final int MESSAGE_TEST_FOR_DISCONNECT = 101;
    private static final long CHECK_ELAPSED_TIME = 3000L;

    private final SparseArray<long[]> devices;

    public View(Context context)
    {
        super(context);

        messageHandler = new Handler() {
            @Override public void handleMessage(Message msg)
            {
                super.handleMessage(msg);
                switch (msg.what)
                {
                    case MESSAGE_TEST_FOR_DISCONNECT:
                    {
                        testForDisconnect();
                        sendEmptyMessageDelayed(MESSAGE_TEST_FOR_DISCONNECT, CHECK_ELAPSED_TIME);
                        break;
                    }
                }
            }
        };

        devices = new SparseArray<long[]>();
        getInputDeviceIds();

        getHolder().addCallback(this);
        setFocusableInTouchMode(true);
        if (android.os.Build.VERSION.SDK_INT >= 19)
            setSystemUiVisibility(SYSTEM_UI_FLAG_LAYOUT_STABLE |
                SYSTEM_UI_FLAG_LOW_PROFILE |
                SYSTEM_UI_FLAG_FULLSCREEN |
                SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    @Override public boolean onTouchEvent(MotionEvent event)
    {
        return OuzelLibJNIWrapper.onTouchEvent(event);
    }

    @Override public boolean onGenericMotionEvent(MotionEvent event)
    {
        return OuzelLibJNIWrapper.onGenericMotionEvent(event);
    }

    @Override public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        onInputEvent(event);
        return OuzelLibJNIWrapper.onKeyDown(keyCode);
    }

    @Override public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        onInputEvent(event);
        return OuzelLibJNIWrapper.onKeyUp(keyCode);
    }

    @Override public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        OuzelLibJNIWrapper.onSurfaceChanged(holder.getSurface(), width, height);
    }

    @Override public void surfaceCreated(SurfaceHolder holder)
    {
        OuzelLibJNIWrapper.onSurfaceCreated(holder.getSurface());
    }

    @Override public void surfaceDestroyed(SurfaceHolder holder)
    {
        OuzelLibJNIWrapper.onSurfaceDestroyed();
    }

    public int[] getInputDeviceIds()
    {
        int[] activeDevices = InputDevice.getDeviceIds();
        long time = SystemClock.elapsedRealtime();
        for (int id : activeDevices)
        {
            long[] lastContact = devices.get(id);
            if (lastContact == null)
            {
                devices.put(id, new long[] { time });
            }
        }
        return activeDevices;
    }

    public void onInputEvent(InputEvent event)
    {
        int id = event.getDeviceId();
        long[] timeArray = devices.get(id);
        if (timeArray == null)
        {
            // device added
            // TODO: implement
            timeArray = new long[1];
            devices.put(id, timeArray);
        }
        long time = SystemClock.elapsedRealtime();
        timeArray[0] = time;
    }

    public void testForDisconnect()
    {
        long time = SystemClock.elapsedRealtime();
        int size = devices.size();
        for (int i = 0; i < size; i++)
        {
            long[] lastContact = devices.valueAt(i);
            if (lastContact != null)
            {
                if (time - lastContact[0] > CHECK_ELAPSED_TIME)
                {
                    int id = devices.keyAt(i);
                    if (InputDevice.getDevice(id) == null)
                    {
                        // device removed
                        // TODO: implement
                        devices.remove(id);
                    }
                    else
                    {
                        lastContact[0] = time;
                    }
                }
            }
        }
    }

    public void onPause()
    {
        messageHandler.removeMessages(MESSAGE_TEST_FOR_DISCONNECT);
    }

    public void onResume()
    {
        messageHandler.sendEmptyMessage(MESSAGE_TEST_FOR_DISCONNECT);
    }
}
