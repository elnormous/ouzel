// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <bcm_host.h>
#include <GLES2/gl2.h>

#include "RendererOGLRPI.h"
#include "core/Engine.h"
#include "WindowRPI.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLRPI::~RendererOGLRPI()
        {
            if (surface)
            {
                if (!eglDestroySurface(display, surface))
                {
                    log("Failed to destroy EGL surface");
                }
            }

            if (context)
            {
                if (!eglDestroyContext(display, context))
                {
                    log("Failed to destroy EGL context");
                }
            }

            if (display)
            {
                if (!eglTerminate(display))
                {
                    log("Failed to terminate EGL");
                }
            }

            bcm_host_deinit();
        }

        void RendererOGLRPI::free()
        {
            if (surface)
            {
                if (!eglDestroySurface(display, surface))
                {
                    log("Failed to destroy EGL surface");
                }
                surface = 0;
            }

            if (context)
            {
                if (!eglDestroyContext(display, context))
                {
                    log("Failed to destroy EGL context");
                }
                context = 0;
            }

            if (display)
            {
                if (!eglTerminate(display))
                {
                    log("Failed to terminate EGL");
                }
                display = 0;
            }
        }

        bool RendererOGLRPI::init(const WindowPtr& window,
                                  uint32_t newSampleCount,
                                  TextureFiltering newTextureFiltering,
                                  float newTargetFPS,
                                  bool newVerticalSync)
        {
            free();

            bcm_host_init();

            display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

            if (!display)
            {
                log("Failed to get display");
                return false;
            }

            if (!eglInitialize(display, NULL, NULL))
            {
                log("Failed to initialize EGL");
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
                log("Failed to choose EGL config");
                return false;
            }

            if (!eglBindAPI(EGL_OPENGL_ES_API))
            {
                log("Failed to bind OpenGL ES API");
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
                log("Failed to create EGL context");
                return false;
            }

            apiVersion = 2;

            uint32_t screenWidth;
            uint32_t screenHeight;
            int32_t success = graphics_get_display_size(0, &screenWidth, &screenHeight);

            if (success == -1)
            {
                log("Failed to get display size");
                return false;
            }

            VC_RECT_T dstRect;
            dstRect.x = 0;
            dstRect.y = 0;
            dstRect.width = screenWidth;
            dstRect.height = screenHeight;

            VC_RECT_T srcRect;
            srcRect.x = 0;
            srcRect.y = 0;
            srcRect.width = screenWidth;
            srcRect.height = screenHeight;

            DISPMANX_DISPLAY_HANDLE_T dispmanDisplay = vc_dispmanx_display_open(0);
            DISPMANX_UPDATE_HANDLE_T dispmanUpdate = vc_dispmanx_update_start(0);

            DISPMANX_ELEMENT_HANDLE_T dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay,
                        0, &dstRect, 0,
                        &srcRect, DISPMANX_PROTECTION_NONE,
                        0, 0, DISPMANX_NO_ROTATE);

            static EGL_DISPMANX_WINDOW_T nativewindow;
            nativewindow.element = dispmanElement;
            nativewindow.width = screenWidth;
            nativewindow.height = screenHeight;
            vc_dispmanx_update_submit_sync(dispmanUpdate);

            surface = eglCreateWindowSurface(display, config, &nativewindow, NULL);
            if (surface == EGL_NO_SURFACE)
            {
                log("Failed to create EGL window surface");
                return false;
            }

            if (!eglMakeCurrent(display, surface, surface, context))
            {
                log("Failed to set current EGL context");
                return false;
            }

            window->setSize(Size2(static_cast<float>(screenWidth),
                                  static_cast<float>(screenHeight)));

            return RendererOGL::init(window, newSampleCount, newTextureFiltering, newTargetFPS, newVerticalSync);
        }

        bool RendererOGLRPI::present()
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
