// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGLAndroid.h"
#include "core/android/ApplicationAndroid.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLAndroid::~RendererOGLAndroid()
        {
        }

        void RendererOGLAndroid::free()
        {
        }

        bool RendererOGLAndroid::init(Window* newWindow,
                                      uint32_t newSampleCount,
                                      TextureFilter newTextureFilter,
                                      PixelFormat newBackBufferFormat,
                                      bool newVerticalSync)
        {
            free();

            ApplicationAndroid* applicationAndroid = static_cast<ApplicationAndroid*>(sharedApplication);
            JNIEnv* jniEnv = applicationAndroid->getJNIEnv();
            jobject mainActivity = applicationAndroid->getMainActivity();
            jclass mainActivityClass = jniEnv->GetObjectClass(mainActivity);
            jmethodID createSurfaceMethod = jniEnv->GetMethodID(mainActivityClass, "createSurface", "(IIIIII)V");
            jniEnv->CallVoidMethod(mainActivity, createSurfaceMethod, 8, 8, 8, 8, 0, 0);

            return RendererOGL::init(newWindow, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync);
        }
    } // namespace graphics
} // namespace ouzel
