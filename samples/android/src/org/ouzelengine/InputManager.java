// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

package org.ouzelengine;

import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.SparseArray;
import android.view.InputDevice;
import android.view.InputEvent;

public class InputManager extends Handler
{
    private static final int MESSAGE_TEST_FOR_DISCONNECT = 101;
    private static final long CHECK_ELAPSED_TIME = 3000L;

    private final SparseArray<long[]> devices;

    @Override public void handleMessage(Message msg)
    {
        super.handleMessage(msg);
        switch (msg.what)
        {
            case MESSAGE_TEST_FOR_DISCONNECT:
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
                sendEmptyMessageDelayed(MESSAGE_TEST_FOR_DISCONNECT, CHECK_ELAPSED_TIME);
                break;
            }
        }
    }

    public InputManager()
    {
        devices = new SparseArray<long[]>();
        getInputDeviceIds();
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

    public void onPause()
    {
        removeMessages(MESSAGE_TEST_FOR_DISCONNECT);
    }

    public void onResume()
    {
        sendEmptyMessage(MESSAGE_TEST_FOR_DISCONNECT);
    }
}
