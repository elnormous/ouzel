// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowAndroid.h"
#include "EngineAndroid.h"
#include "utils/Log.h"

namespace ouzel
{
    WindowAndroid::WindowAndroid()
    {
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(sharedEngine);
        JNIEnv* jniEnv;

        if (engineAndroid->getJavaVM()->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return;
        }

        window = ANativeWindow_fromSurface(jniEnv, engineAndroid->getSurface());
    }

    WindowAndroid::~WindowAndroid()
    {
        if (window)
        {
            ANativeWindow_release(window);
        }
    }

    void WindowAndroid::handleResize(const Size2& newSize)
    {
        Event event;
        event.type = Event::Type::WINDOW_SIZE_CHANGE;

        event.windowEvent.window = this;
        event.windowEvent.size = newSize;

        sharedEngine->getEventDispatcher()->postEvent(event);
    }
}
