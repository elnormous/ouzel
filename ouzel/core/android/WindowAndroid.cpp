// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowAndroid.h"
#include "ApplicationAndroid.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    WindowAndroid::WindowAndroid()
    {
        ApplicationAndroid* applicationAndroid = static_cast<ApplicationAndroid*>(sharedApplication);
        JNIEnv* jniEnv;

        if (applicationAndroid->getJavaVM()->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
        {
            Log(Log::Level::ERR) << "Failed to get JNI environment";
            return;
        }

        window = ANativeWindow_fromSurface(jniEnv, applicationAndroid->getSurface());
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
