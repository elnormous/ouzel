// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <GLES2/gl2.h>

#include "RendererOGLRasp.h"
#include "core/Engine.h"
#include "core/raspbian/WindowRasp.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLRasp::~RendererOGLRasp()
        {
            if (surface)
            {
                if (!eglDestroySurface(display, surface))
                {
                    Log(Log::Level::ERR) << "Failed to destroy EGL surface";
                }
            }

            if (context)
            {
                if (!eglDestroyContext(display, context))
                {
                    Log(Log::Level::ERR) << "Failed to destroy EGL context";
                }
            }

            if (display)
            {
                if (!eglTerminate(display))
                {
                    Log(Log::Level::ERR) << "Failed to terminate EGL";
                }
            }
        }

        void RendererOGLRasp::free()
        {
            if (surface)
            {
                if (!eglDestroySurface(display, surface))
                {
                    Log(Log::Level::ERR) << "Failed to destroy EGL surface";
                }
                surface = 0;
            }

            if (context)
            {
                if (!eglDestroyContext(display, context))
                {
                    Log(Log::Level::ERR) << "Failed to destroy EGL context";
                }
                context = 0;
            }

            if (display)
            {
                if (!eglTerminate(display))
                {
                    Log(Log::Level::ERR) << "Failed to terminate EGL";
                }
                display = 0;
            }
        }

        bool RendererOGLRasp::init(Window* newWindow,
                                   uint32_t newSampleCount,
                                   TextureFilter newTextureFilter,
                                   PixelFormat newBackBufferFormat,
                                   bool newVerticalSync)
        {
            free();

            display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

            if (!display)
            {
                Log(Log::Level::ERR) << "Failed to get display";
                return false;
            }

            if (!eglInitialize(display, NULL, NULL))
            {
                Log(Log::Level::ERR) << "Failed to initialize EGL";
                return false;
            }

            static const EGLint attributeList[] =
            {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
            };
            EGLConfig config;
            EGLint numConfig;
            if (!eglChooseConfig(display, attributeList, &config, 1, &numConfig))
            {
                Log(Log::Level::ERR) << "Failed to choose EGL config";
                return false;
            }

            if (!eglBindAPI(EGL_OPENGL_ES_API))
            {
                Log(Log::Level::ERR) << "Failed to bind OpenGL ES API";
                return false;
            }

            // create an EGL rendering context
            static const EGLint contextAttributes[] =
            {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE
            };
            context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);

            if (context == EGL_NO_CONTEXT)
            {
                Log(Log::Level::ERR) << "Failed to create EGL context";
                return false;
            }

            apiMajorVersion = 2;
            apiMinorVersion = 0;

            uint32_t width = static_cast<uint32_t>(newWindow->getSize().width);
            uint32_t height = static_cast<uint32_t>(newWindow->getSize().height);

            VC_RECT_T dstRect;
            dstRect.x = 0;
            dstRect.y = 0;
            dstRect.width = width;
            dstRect.height = height;

            VC_RECT_T srcRect;
            srcRect.x = 0;
            srcRect.y = 0;
            srcRect.width = width;
            srcRect.height = height;

            DISPMANX_DISPLAY_HANDLE_T dispmanDisplay = vc_dispmanx_display_open(0);
            DISPMANX_UPDATE_HANDLE_T dispmanUpdate = vc_dispmanx_update_start(0);

            DISPMANX_ELEMENT_HANDLE_T dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay,
                        0, &dstRect, 0,
                        &srcRect, DISPMANX_PROTECTION_NONE,
                        0, 0, DISPMANX_NO_ROTATE);

            static EGL_DISPMANX_WINDOW_T nativewindow;
            nativewindow.element = dispmanElement;
            nativewindow.width = width;
            nativewindow.height = height;
            vc_dispmanx_update_submit_sync(dispmanUpdate);

            surface = eglCreateWindowSurface(display, config, &nativewindow, NULL);
            if (surface == EGL_NO_SURFACE)
            {
                Log(Log::Level::ERR) << "Failed to create EGL window surface";
                return false;
            }

            if (!eglMakeCurrent(display, surface, surface, context))
            {
                Log(Log::Level::ERR) << "Failed to set current EGL context";
                return false;
            }

            if (!eglSwapInterval(display, newVerticalSync ? 1 : 0))
            {
                Log(Log::Level::ERR) << "Failed to set EGL frame interval";
                return false;
            }

            return RendererOGL::init(newWindow, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync);
        }

        bool RendererOGLRasp::present()
        {
            if (!RendererOGL::present())
            {
                return false;
            }

            if (eglSwapBuffers(display, surface) != EGL_TRUE)
            {
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
