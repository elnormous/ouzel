// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowResourceAndroid.hpp"
#include "EngineAndroid.hpp"
#include "thread/Lock.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    WindowResourceAndroid::WindowResourceAndroid()
    {
    }

    WindowResourceAndroid::~WindowResourceAndroid()
    {
        if (window) ANativeWindow_release(window);
    }

    bool WindowResourceAndroid::init(const Size2& newSize,
                                     bool newResizable,
                                     bool newFullscreen,
                                     bool newExclusiveFullscreen,
                                     const std::string& newTitle,
                                     bool newHighDpi,
                                     bool depth)
    {
        if (!WindowResource::init(newSize,
                                  newResizable,
                                  newFullscreen,
                                  newExclusiveFullscreen,
                                  newTitle,
                                  newHighDpi,
                                  depth))
            return false;

        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);
        JavaVM* javaVM = engineAndroid->getJavaVM();
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw SystemError("Failed to get JNI environment");

        window = ANativeWindow_fromSurface(jniEnv, engineAndroid->getSurface());

        size.width = static_cast<float>(ANativeWindow_getWidth(window));
        size.height = static_cast<float>(ANativeWindow_getHeight(window));
        resolution = size;

        return true;
    }

    void WindowResourceAndroid::handleResize(const Size2& newSize)
    {
        size = newSize;
        resolution = size;

        Lock lock(listenerMutex);
        if (listener)
        {
            listener->onSizeChange(size);
            listener->onResolutionChange(resolution);
        }
    }

    void WindowResourceAndroid::handleSurfaceChange(jobject surface)
    {
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);
        JavaVM* javaVM = engineAndroid->getJavaVM();
        JNIEnv* jniEnv;

        if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            throw SystemError("Failed to get JNI environment");

        if (window) ANativeWindow_release(window);
        window = ANativeWindow_fromSurface(jniEnv, surface);
    }

    void WindowResourceAndroid::handleSurfaceDestroy()
    {
        if (window)
        {
            ANativeWindow_release(window);
            window = nullptr;
        }
    }
}
