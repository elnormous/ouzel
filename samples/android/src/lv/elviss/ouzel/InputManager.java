// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;
import android.util.SparseArray;
import android.view.InputDevice;
import android.view.InputEvent;
import android.view.MotionEvent;

import java.lang.ref.WeakReference;
import java.util.ArrayDeque;
import java.util.HashMap;
import java.util.Map;
import java.util.Queue;

public class InputManager
{
    private static final int MESSAGE_TEST_FOR_DISCONNECT = 101;
    private static final long CHECK_ELAPSED_TIME = 3000L;

    private static final int ON_DEVICE_ADDED = 0;
    private static final int ON_DEVICE_CHANGED = 1;
    private static final int ON_DEVICE_REMOVED = 2;

    private final SparseArray<long[]> devices;
    private final Map<InputDeviceListener, Handler> listeners;
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
                            if (null != lastContact)
                            {
                                if (time - lastContact[0] > CHECK_ELAPSED_TIME)
                                {
                                    int id = imv.devices.keyAt(i);
                                    if (null == InputDevice.getDevice(id))
                                    {
                                        imv.notifyListeners(ON_DEVICE_REMOVED, id);
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
        listeners = new HashMap<InputDeviceListener, Handler>();
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

    public void registerInputDeviceListener(InputDeviceListener listener, Handler handler)
    {
        listeners.remove(listener);
        if (handler == null)
        {
            handler = defaultHandler;
        }
        listeners.put(listener, handler);
    }

    public void unregisterInputDeviceListener(InputDeviceListener listener)
    {
        listeners.remove(listener);
    }

    private void notifyListeners(int why, int deviceId)
    {
        if (!listeners.isEmpty())
        {
            for (InputDeviceListener listener : listeners.keySet())
            {
                Handler handler = listeners.get(listener);
                DeviceEvent odc = DeviceEvent.getDeviceEvent(why, deviceId, listener);
                handler.post(odc);
            }
        }
    }

    private static class DeviceEvent implements Runnable
    {
        private int messageType;
        private int id;
        private InputDeviceListener listener;
        private static Queue<DeviceEvent> eventQueue = new ArrayDeque<DeviceEvent>();

        private DeviceEvent()
        {
        }

        static DeviceEvent getDeviceEvent(int messageType, int id, InputDeviceListener listener)
        {
            DeviceEvent curChanged = eventQueue.poll();
            if (curChanged == null)
            {
                curChanged = new DeviceEvent();
            }
            curChanged.messageType = messageType;
            curChanged.id = id;
            curChanged.listener = listener;
            return curChanged;
        }

        @Override
        public void run()
        {
            switch (messageType)
            {
                case ON_DEVICE_ADDED:
                    listener.onInputDeviceAdded(id);
                    break;
                case ON_DEVICE_CHANGED:
                    listener.onInputDeviceChanged(id);
                    break;
                case ON_DEVICE_REMOVED:
                    listener.onInputDeviceRemoved(id);
                    break;
                default:
                    Log.e("Ouzel", "Unknown Message Type");
                    break;
            }

            eventQueue.offer(this);
        }
    }

    public void onInputEvent(InputEvent event)
    {
        int id = event.getDeviceId();
        long[] timeArray = devices.get(id);
        if (timeArray == null)
        {
            notifyListeners(ON_DEVICE_ADDED, id);
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
