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
                                      const Size2& newSize,
                                      uint32_t newSampleCount,
                                      TextureFilter newTextureFilter,
                                      PixelFormat newBackBufferFormat,
                                      bool newVerticalSync,
                                      uint32_t newDepthBits)
        {
            free();

            ApplicationAndroid* applicationAndroid = static_cast<ApplicationAndroid*>(sharedApplication);
            JNIEnv* jniEnv = applicationAndroid->getJNIEnv();
            jobject mainActivity = applicationAndroid->getMainActivity();
            jclass mainActivityClass = jniEnv->GetObjectClass(mainActivity);
            jmethodID createSurfaceMethod = jniEnv->GetMethodID(mainActivityClass, "createSurface", "(IIIIII)V");
            jniEnv->CallVoidMethod(mainActivity, createSurfaceMethod, 8, 8, 8, 8, newDepthBits, 0);

            apiMajorVersion = 2;
            apiMinorVersion = 0;

            return RendererOGL::init(newWindow, newSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepthBits);
        }
    } // namespace graphics
} // namespace ouzel
