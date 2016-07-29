// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

package lv.elviss.ouzel;

public interface InputDeviceListener
{
    void onInputDeviceAdded(int deviceId);
    void onInputDeviceChanged(int deviceId);
    void onInputDeviceRemoved(int deviceId);
}
