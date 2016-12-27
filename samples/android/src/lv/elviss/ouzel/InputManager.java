// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.SparseArray;
import android.view.InputDevice;
import android.view.InputEvent;
import android.view.MotionEvent;
import java.lang.ref.WeakReference;

public class InputManager
{
    private static final int MESSAGE_TEST_FOR_DISCONNECT = 101;
    private static final long CHECK_ELAPSED_TIME = 3000L;

    private final SparseArray<long[]> devices;
    private final Handler defaultHandler;

    private static class PollingMessageHandler extends Handler
    {
        private final WeakReference<InputManager> inputManager;

        PollingMessageHandler(InputManager im)
        {
            inputManager = new WeakReference<InputManager>(im);
        }

        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            switch (msg.what)
            {
                case MESSAGE_TEST_FOR_DISCONNECT:
                {
                    InputManager imv = inputManager.get();
                    if (imv != null)
                    {
                        long time = SystemClock.elapsedRealtime();
                        int size = imv.devices.size();
                        for (int i = 0; i < size; i++)
                        {
                            long[] lastContact = imv.devices.valueAt(i);
                            if (lastContact != null)
                            {
                                if (time - lastContact[0] > CHECK_ELAPSED_TIME)
                                {
                                    int id = imv.devices.keyAt(i);
                                    if (InputDevice.getDevice(id) == null)
                                    {
                                        // device removed
                                        // TODO: implement
                                        imv.devices.remove(id);
                                    }
                                    else
                                    {
                                        lastContact[0] = time;
                                    }
                                }
                            }
                        }
                        sendEmptyMessageDelayed(MESSAGE_TEST_FOR_DISCONNECT, CHECK_ELAPSED_TIME);
                    }
                    break;
                }
            }
        }
    }

    public InputManager()
    {
        devices = new SparseArray<long[]>();
        defaultHandler = new PollingMessageHandler(this);
        getInputDeviceIds();
    }

    public InputDevice getInputDevice(int id)
    {
        return InputDevice.getDevice(id);
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
        defaultHandler.removeMessages(MESSAGE_TEST_FOR_DISCONNECT);
    }

    public void onResume()
    {
        defaultHandler.sendEmptyMessage(MESSAGE_TEST_FOR_DISCONNECT);
    }
}
