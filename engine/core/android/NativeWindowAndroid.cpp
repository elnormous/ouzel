// Ouzel by Elviss Strazdins

#include <stdexcept>
#include "NativeWindowAndroid.hpp"
#include "JniErrorCategory.hpp"
#include "EngineAndroid.hpp"

namespace ouzel::core::android
{
    NativeWindow::NativeWindow(const std::string& newTitle):
        core::NativeWindow{
            math::Size<std::uint32_t, 2>(),
            true,
            true,
            true,
            newTitle,
            true
        }
    {
        const auto engineAndroid = static_cast<Engine*>(engine);
        const auto javaVm = engineAndroid->getJavaVm();
        void* jniEnvPointer;

        if (const auto result = javaVm->GetEnv(&jniEnvPointer, JNI_VERSION_1_6); result != JNI_OK)
            throw std::system_error{result, errorCategory, "Failed to get JNI environment"};

        const auto jniEnv = static_cast<JNIEnv*>(jniEnvPointer);

        window = ANativeWindow_fromSurface(jniEnv, engineAndroid->getSurface());

        size.v[0] = static_cast<std::uint32_t>(ANativeWindow_getWidth(window));
        size.v[1] = static_cast<std::uint32_t>(ANativeWindow_getHeight(window));
        resolution = size;
    }

    NativeWindow::~NativeWindow()
    {
        if (window) ANativeWindow_release(window);
    }

    void NativeWindow::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::changeSize:
                break;
            case Command::Type::changeFullscreen:
                // TODO: implement (hide status bar and UI buttons)
                break;
            case Command::Type::close:
                break;
            case Command::Type::setTitle:
                break;
            case Command::Type::bringToFront:
                break;
            case Command::Type::show:
                break;
            case Command::Type::hide:
                break;
            case Command::Type::minimize:
                break;
            case Command::Type::maximize:
                break;
            case Command::Type::restore:
                break;
            default:
                throw std::runtime_error{"Invalid command"};
        }
    }

    void NativeWindow::handleResize(const math::Size<std::uint32_t, 2>& newSize)
    {
        size = newSize;
        resolution = size;

        Event sizeChangeEvent{Event::Type::sizeChange};
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent{Event::Type::resolutionChange};
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindow::handleSurfaceChange(jobject surface)
    {
        const auto engineAndroid = static_cast<Engine*>(engine);
        const auto javaVm = engineAndroid->getJavaVm();
        void* jniEnvPointer;

        if (const auto result = javaVm->GetEnv(&jniEnvPointer, JNI_VERSION_1_6); result != JNI_OK)
            throw std::system_error{result, errorCategory, "Failed to get JNI environment"};

        const auto jniEnv = static_cast<JNIEnv*>(jniEnvPointer);

        if (window) ANativeWindow_release(window);
        window = ANativeWindow_fromSurface(jniEnv, surface);
    }

    void NativeWindow::handleSurfaceDestroy()
    {
        if (window)
        {
            ANativeWindow_release(window);
            window = nullptr;
        }
    }
}
