// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "NativeWindowAndroid.hpp"
#include "EngineAndroid.hpp"

namespace ouzel
{
    NativeWindowAndroid::NativeWindowAndroid(const std::function<void(const Event&)>& initCallback,
                                             const std::string& newTitle):
        NativeWindow(initCallback,
                     Size2U(),
                     true,
                     true,
                     true,
                     newTitle,
                     true)
    {
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);
        JavaVM* javaVM = engineAndroid->getJavaVM();
        JNIEnv* jniEnv;

        jint result;
        if ((result = javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6)) != JNI_OK)
            throw std::system_error(result, jniErrorCategory, "Failed to get JNI environment");

        window = ANativeWindow_fromSurface(jniEnv, engineAndroid->getSurface());

        size.v[0] = static_cast<uint32_t>(ANativeWindow_getWidth(window));
        size.v[1] = static_cast<uint32_t>(ANativeWindow_getHeight(window));
        resolution = size;
    }

    NativeWindowAndroid::~NativeWindowAndroid()
    {
        if (window) ANativeWindow_release(window);
    }

    void NativeWindowAndroid::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::ChangeSize:
                break;
            case Command::Type::ChangeFullscreen:
                // TODO: implement (hide status bar and UI buttons)
                break;
            case Command::Type::Close:
                break;
            case Command::Type::SetTitle:
                break;
            case Command::Type::BringToFront:
                break;
            case Command::Type::Show:
                break;
            case Command::Type::Hide:
                break;
            case Command::Type::Minimize:
                break;
            case Command::Type::Maximize:
                break;
            case Command::Type::Restore:
                break;
            default:
                throw std::runtime_error("Invalid command");
        }
    }

    void NativeWindowAndroid::handleResize(const Size2U& newSize)
    {
        size = newSize;
        resolution = size;

        Event sizeChangeEvent(Event::Type::SIZE_CHANGE);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindowAndroid::handleSurfaceChange(jobject surface)
    {
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);
        JavaVM* javaVM = engineAndroid->getJavaVM();
        JNIEnv* jniEnv;

        jint result;
        if ((result = javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6)) != JNI_OK)
            throw std::system_error(result, jniErrorCategory, "Failed to get JNI environment");

        if (window) ANativeWindow_release(window);
        window = ANativeWindow_fromSurface(jniEnv, surface);
    }

    void NativeWindowAndroid::handleSurfaceDestroy()
    {
        if (window)
        {
            ANativeWindow_release(window);
            window = nullptr;
        }
    }
}
