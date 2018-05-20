// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_PLATFORM_WINDOWS && OUZEL_COMPILE_OPENGL

#include <cstring>
#define GL_GLEXT_PROTOTYPES 1
#include "GL/glcorearb.h"
#include "GL/glext.h"
#include "GL/wglext.h"
#include "RenderDeviceOGLWin.hpp"
#include "core/Engine.hpp"
#include "core/windows/WindowResourceWin.hpp"

static const LPCWSTR TEMP_WINDOW_CLASS_NAME = L"TempWindow";

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(window, msg, wParam, lParam);
}

namespace ouzel
{
    namespace graphics
    {
        class TempContext
        {
        public:
            TempContext()
            {
                HINSTANCE hInstance = GetModuleHandleW(nullptr);

                WNDCLASSW wc;
                wc.style = CS_OWNDC;
                wc.lpfnWndProc = windowProc;
                wc.cbClsExtra = 0;
                wc.cbWndExtra = 0;
                wc.hInstance = hInstance;
                wc.hIcon = 0;
                wc.hCursor = 0;
                wc.hbrBackground = 0;
                wc.lpszMenuName = nullptr;
                wc.lpszClassName = TEMP_WINDOW_CLASS_NAME;

                windowClass = RegisterClassW(&wc);

                if (!windowClass)
                {
                    Log(Log::Level::ERR) << "Failed to register window class";
                    return;
                }

                window = CreateWindowW(TEMP_WINDOW_CLASS_NAME, L"TempWindow", 0,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       0, 0, hInstance, 0);

                if (!window)
                {
                    Log(Log::Level::ERR) << "Failed to create window";
                    return;
                }

                deviceContext = GetDC(window);

                PIXELFORMATDESCRIPTOR pixelFormatDesc;
                pixelFormatDesc.nSize = sizeof(pixelFormatDesc);
                pixelFormatDesc.nVersion = 1;
                pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
                pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
                pixelFormatDesc.cColorBits = 24;
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
                pixelFormatDesc.cDepthBits = 0;
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
                    return;
                }

                if (!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc))
                {
                    Log(Log::Level::ERR) << "Failed to set pixel format";
                    return;
                }

                renderContext = wglCreateContext(deviceContext);

                if (!renderContext)
                {
                    Log(Log::Level::ERR) << "Failed to create OpenGL context";
                    return;
                }

                if (!wglMakeCurrent(deviceContext, renderContext))
                {
                    Log(Log::Level::ERR) << "Failed to set current OpenGL context";
                    return;
                }
            }

            ~TempContext()
            {
                if (renderContext)
                {
                    if (!wglMakeCurrent(deviceContext, nullptr))
                        Log(Log::Level::ERR) << "Failed to unset OpenGL context";

                    if (!wglDeleteContext(renderContext))
                        Log(Log::Level::ERR) << "Failed to delete OpenGL context";
                }

                if (window)
                    DestroyWindow(window);

                if (windowClass)
                    UnregisterClassW(TEMP_WINDOW_CLASS_NAME, GetModuleHandleW(nullptr));
            }

        private:
            ATOM windowClass = 0;
            HWND window = 0;
            HDC deviceContext = 0;
            HGLRC renderContext = 0;
        };

        RenderDeviceOGLWin::RenderDeviceOGLWin():
            running(false)
        {
        }

        RenderDeviceOGLWin::~RenderDeviceOGLWin()
        {
            running = false;
            flushCommands();

            if (renderThread.isJoinable()) renderThread.join();

            if (renderContext)
            {
                if (!wglMakeCurrent(deviceContext, nullptr))
                    Log(Log::Level::ERR) << "Failed to unset OpenGL context";

                if (!wglDeleteContext(renderContext))
                    Log(Log::Level::ERR) << "Failed to delete OpenGL context";
            }
        }

        bool RenderDeviceOGLWin::init(Window* newWindow,
                                      const Size2& newSize,
                                      uint32_t newSampleCount,
                                      Texture::Filter newTextureFilter,
                                      uint32_t newMaxAnisotropy,
                                      bool newVerticalSync,
                                      bool newDepth,
                                      bool newDebugRenderer)
        {
            TempContext tempContext;

            PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatProc = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));

            WindowResourceWin* windowWin = static_cast<WindowResourceWin*>(newWindow->getResource());

            deviceContext = GetDC(windowWin->getNativeWindow());

            if (!deviceContext)
            {
                Log(Log::Level::ERR) << "Failed to get window's device context";
                return false;
            }

            int pixelFormat = 0;

            PIXELFORMATDESCRIPTOR pixelFormatDesc;
            pixelFormatDesc.nSize = sizeof(pixelFormatDesc);
            pixelFormatDesc.nVersion = 1;
            pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
            pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
            pixelFormatDesc.cColorBits = 24;
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

            if (wglChoosePixelFormatProc)
            {
                const int attributeList[] =
                {
                    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                    WGL_COLOR_BITS_ARB, 24,
                    WGL_ALPHA_BITS_ARB, 8,
                    WGL_DEPTH_BITS_ARB, newDepth ? 24 : 0,
                    WGL_SAMPLE_BUFFERS_ARB, newSampleCount > 0 ? 1 : 0,
                    WGL_SAMPLES_ARB, static_cast<int>(newSampleCount),
                    0,
                };

                UINT numFormats;

                if (!wglChoosePixelFormatProc(deviceContext, attributeList, nullptr, 1, &pixelFormat, &numFormats))
                {
                    Log(Log::Level::ERR) << "Failed to choose pixel format";
                    return false;
                }
            }
            else
            {
                pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
            }

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

            PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsProc = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));

            if (wglCreateContextAttribsProc)
            {
                for (int openGLVersion = 4; openGLVersion >= 2; --openGLVersion)
                {
                    std::vector<int> contextAttribs = {
                        WGL_CONTEXT_MAJOR_VERSION_ARB, openGLVersion,
                        WGL_CONTEXT_MINOR_VERSION_ARB, 0,
                    };

                    if (newDebugRenderer)
                    {
                        contextAttribs.push_back(WGL_CONTEXT_FLAGS_ARB);
                        contextAttribs.push_back(WGL_CONTEXT_DEBUG_BIT_ARB);
                    }

                    contextAttribs.push_back(0);

                    renderContext = wglCreateContextAttribsProc(deviceContext, 0, contextAttribs.data());

                    if (renderContext)
                    {
                        Log(Log::Level::INFO) << "OpenGL " << openGLVersion << " context created";
                        break;
                    }
                }
            }
            else
            {
                renderContext = wglCreateContext(deviceContext);
            }

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

            apiMajorVersion = static_cast<uint16_t>(stoi(majorVersion));
            apiMinorVersion = static_cast<uint16_t>(stoi(minorVersion));

            if (apiMajorVersion < 2 ||
                apiMajorVersion > 4)
            {
                Log(Log::Level::ERR) << "Unsupported OpenGL version";
                return false;
            }

            if (!RenderDeviceOGL::init(newWindow,
                                       newSize,
                                       newSampleCount,
                                       newTextureFilter,
                                       newMaxAnisotropy,
                                       newVerticalSync,
                                       newDepth,
                                       newDebugRenderer))
            {
                return false;
            }

            if (!wglMakeCurrent(deviceContext, nullptr))
            {
                Log(Log::Level::ERR) << "Failed to unset OpenGL context";
                return false;
            }

            running = true;
            renderThread = Thread(std::bind(&RenderDeviceOGLWin::main, this), "Render");

            return true;
        }

        bool RenderDeviceOGLWin::lockContext()
        {
            if (!wglMakeCurrent(deviceContext, renderContext))
            {
                Log(Log::Level::ERR) << "Failed to set current OpenGL context";
                return false;
            }

            return true;
        }

        bool RenderDeviceOGLWin::swapBuffers()
        {
            if (!SwapBuffers(deviceContext))
            {
                Log(Log::Level::ERR) << "Failed to swap buffers";
                return false;
            }

            return true;
        }

        void RenderDeviceOGLWin::main()
        {
            while (running) process();
        }
    } // namespace graphics
} // namespace ouzel

#endif
