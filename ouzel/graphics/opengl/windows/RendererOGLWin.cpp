// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#define NOMINMAX
#define GL_GLEXT_PROTOTYPES 1
#include "GL/glcorearb.h"
#include "GL/glext.h"
#include "GL/wglext.h"
#include "RendererOGLWin.h"
#include "core/windows/WindowWin.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLWin::~RendererOGLWin()
        {
            if (renderContext)
            {
                wglMakeCurrent(deviceContext, nullptr);
                wglDeleteContext(renderContext);
            }
        }

        bool RendererOGLWin::init(Window* newWindow,
                                  const Size2& newSize,
                                  uint32_t newSampleCount,
                                  Texture::Filter newTextureFilter,
                                  PixelFormat newBackBufferFormat,
                                  bool newVerticalSync,
                                  bool newDepth)
        {
            WindowWin* windowWin = static_cast<WindowWin*>(newWindow);

            deviceContext = GetDC(windowWin->getNativeWindow());

            if (!deviceContext)
            {
                Log(Log::Level::ERR) << "Failed to get window's device context";
                return false;
            }

            PIXELFORMATDESCRIPTOR pixelFormatDesc;
            pixelFormatDesc.nSize = sizeof(pixelFormatDesc);
            pixelFormatDesc.nVersion = 1;
            pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
            pixelFormatDesc.cColorBits = 32;
            pixelFormatDesc.cRedBits = 0;
            pixelFormatDesc.cRedShift = 0;
            pixelFormatDesc.cGreenBits = 0;
            pixelFormatDesc.cGreenShift = 0;
            pixelFormatDesc.cBlueBits = 0;
            pixelFormatDesc.cBlueShift = 0;
            pixelFormatDesc.cAlphaBits = 0;
            pixelFormatDesc.cAlphaShift = 0;
            pixelFormatDesc.cAccumBits = 0;
            pixelFormatDesc.cAccumRedBits = 0;
            pixelFormatDesc.cAccumGreenBits = 0;
            pixelFormatDesc.cAccumBlueBits = 0;
            pixelFormatDesc.cAccumAlphaBits = 0;
            pixelFormatDesc.cDepthBits = newDepth ? 24 : 0;
            pixelFormatDesc.cStencilBits = 0;
            pixelFormatDesc.cAuxBuffers = 0;
            pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;
            pixelFormatDesc.bReserved = 0;
            pixelFormatDesc.dwLayerMask = 0;
            pixelFormatDesc.dwVisibleMask = 0;
            pixelFormatDesc.dwDamageMask = 0;

            int pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);

            if (!pixelFormat)
            {
                Log(Log::Level::ERR) << "Failed to choose pixel format";
                return false;
            }

            if (!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc))
            {
                Log(Log::Level::ERR) << "Failed to set pixel format";
                return false;
            }

            renderContext = wglCreateContext(deviceContext);

            if (!renderContext)
            {
                Log(Log::Level::ERR) << "Failed to create OpenGL context";
                return false;
            }

            if (!wglMakeCurrent(deviceContext, renderContext))
            {
                Log(Log::Level::ERR) << "Failed to set current OpenGL context";
                return false;
            }

            const GLubyte* versionPtr = glGetString(GL_VERSION);

            if (!versionPtr)
            {
                Log(Log::Level::ERR) << "Failed to get OpenGL version";
                return false;
            }

            std::string version(reinterpret_cast<const char*>(versionPtr));
            std::string majorVersion;
            std::string minorVersion;
            uint32_t part = 0;

            for (char c : version)
            {
                if (c == '.' || c == ' ')
                {
                    if (++part > 1) break;
                }
                else if (part == 0)
                    majorVersion += c;
                else if (part == 1)
                    minorVersion += c;
            }

            apiMajorVersion = fromString<uint16_t>(majorVersion);
            apiMinorVersion = fromString<uint16_t>(minorVersion);

            if (apiMajorVersion < 2)
            {
                Log(Log::Level::ERR) << "Unsupported OpenGL version";
                return false;
            }

            return RendererOGL::init(newWindow, newSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepth);
        }

        bool RendererOGLWin::lockContext()
        {
            if (!wglMakeCurrent(deviceContext, renderContext))
            {
                Log(Log::Level::ERR) << "Failed to set current OpenGL context";
                return false;
            }

            return true;
        }

        bool RendererOGLWin::swapBuffers()
        {
            if (!SwapBuffers(deviceContext))
            {
                Log(Log::Level::ERR) << "Failed to swap buffers";
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
