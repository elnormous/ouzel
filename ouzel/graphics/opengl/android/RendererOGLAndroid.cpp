// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGLAndroid.h"
#include "core/android/ApplicationAndroid.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLAndroid::~RendererOGLAndroid()
        {
        }

        bool RendererOGLAndroid::init(Window* newWindow,
                                      const Size2& newSize,
                                      uint32_t newSampleCount,
                                      Texture::Filter newTextureFilter,
                                      PixelFormat newBackBufferFormat,
                                      bool newVerticalSync,
                                      bool newDepth)
        {
            ApplicationAndroid* applicationAndroid = static_cast<ApplicationAndroid*>(sharedApplication);
            JavaVM* javaVM = applicationAndroid->getJavaVM();
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            {
                Log(Log::Level::ERR) << "Failed to get JNI environment";
            }

            jobject mainActivity = applicationAndroid->getMainActivity();
            jmethodID createSurfaceMethod = applicationAndroid->getCreateSurfaceMethod();
            jniEnv->CallVoidMethod(mainActivity, createSurfaceMethod, 8, 8, 8, 8, newDepth ? 24 : 0, 0, (newSampleCount > 1) ? 1 : 0, newSampleCount);

            apiMajorVersion = 2;
            apiMinorVersion = 0;

            return RendererOGL::init(newWindow, newSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepth);
        }
    } // namespace graphics
} // namespace ouzel
