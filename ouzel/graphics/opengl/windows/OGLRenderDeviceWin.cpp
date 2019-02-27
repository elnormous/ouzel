// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if defined(_WIN32) && OUZEL_COMPILE_OPENGL

#include <system_error>
#include "GL/glcorearb.h"
#include "GL/glext.h"
#include "GL/wglext.h"
#include "OGLRenderDeviceWin.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

static constexpr LPCWSTR TEMP_WINDOW_CLASS_NAME = L"TempWindow";

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(window, msg, wParam, lParam);
}

namespace ouzel
{
    namespace graphics
    {
        class TempContext final
        {
        public:
            TempContext()
            {
                HINSTANCE instance = GetModuleHandleW(nullptr);
                if (!instance)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get module handle");

                WNDCLASSW wc;
                wc.style = CS_OWNDC;
                wc.lpfnWndProc = windowProc;
                wc.cbClsExtra = 0;
                wc.cbWndExtra = 0;
                wc.hInstance = instance;
                wc.hIcon = 0;
                wc.hCursor = 0;
                wc.hbrBackground = 0;
                wc.lpszMenuName = nullptr;
                wc.lpszClassName = TEMP_WINDOW_CLASS_NAME;

                windowClass = RegisterClassW(&wc);

                if (!windowClass)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to register window class");

                window = CreateWindowW(TEMP_WINDOW_CLASS_NAME, L"TempWindow", 0,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       0, 0, instance, 0);

                if (!window)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create window");

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
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to choose pixel format");

                if (!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to set pixel format");

                renderContext = wglCreateContext(deviceContext);

                if (!renderContext)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create OpenGL context");

                if (!wglMakeCurrent(deviceContext, renderContext))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to set current OpenGL context");
            }

            ~TempContext()
            {
                if (renderContext)
                {
                    wglMakeCurrent(deviceContext, nullptr);
                    wglDeleteContext(renderContext);
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

        OGLRenderDeviceWin::OGLRenderDeviceWin(const std::function<void(const Event&)>& initCallback):
            OGLRenderDevice(initCallback)
        {
        }

        OGLRenderDeviceWin::~OGLRenderDeviceWin()
        {
            running = false;
            CommandBuffer commandBuffer;
            commandBuffer.pushCommand(std::unique_ptr<Command>(new PresentCommand()));
            submitCommandBuffer(std::move(commandBuffer));

            if (renderThread.joinable()) renderThread.join();

            if (renderContext)
            {
                wglMakeCurrent(deviceContext, nullptr);
                wglDeleteContext(renderContext);
            }
        }

        void OGLRenderDeviceWin::init(Window* newWindow,
                                      const Size<2, uint32_t>& newSize,
                                      uint32_t newSampleCount,
                                      Texture::Filter newTextureFilter,
                                      uint32_t newMaxAnisotropy,
                                      bool newVerticalSync,
                                      bool newDepth,
                                      bool newStencil,
                                      bool newDebugRenderer)
        {
            TempContext tempContext;

            NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(newWindow->getNativeWindow());

            deviceContext = GetDC(windowWin->getNativeWindow());
            if (!deviceContext)
                throw std::runtime_error("Failed to get window's device context");

            std::vector<std::string> extensions;

            if (PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringProc = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB")))
            {
                if (const char* extensionsPtr = wglGetExtensionsStringProc(deviceContext))
                    extensions = explodeString(reinterpret_cast<const char*>(extensionsPtr), ' ');

                engine->log(Log::Level::ALL) << "Supported WGL extensions: " << extensions;
            }

            PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatProc = nullptr;
            PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsProc = nullptr;

            for (const std::string& extension : extensions)
            {
                if (extension == "WGL_ARB_pixel_format")
                    wglChoosePixelFormatProc = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
                else if (extension == "WGL_ARB_create_context")
                    wglCreateContextAttribsProc = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
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
                    WGL_STENCIL_BITS_ARB, newStencil ? 8 : 0,
                    WGL_SAMPLE_BUFFERS_ARB, newSampleCount > 0 ? 1 : 0,
                    WGL_SAMPLES_ARB, static_cast<int>(newSampleCount),
                    0,
                };

                UINT numFormats;

                if (!wglChoosePixelFormatProc(deviceContext, attributeList, nullptr, 1, &pixelFormat, &numFormats))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to choose pixel format");
            }
            else
            {
                pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
                if (!pixelFormat)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to choose pixel format");
            }

            if (!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to set pixel format");

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
                        engine->log(Log::Level::INFO) << "OpenGL " << openGLVersion << " context created";
                        break;
                    }
                }
            }
            else
                renderContext = wglCreateContext(deviceContext);

            if (!renderContext)
                throw std::runtime_error("Failed to create OpenGL context");

            if (!wglMakeCurrent(deviceContext, renderContext))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to set current OpenGL context");

            OGLRenderDevice::init(newWindow,
                                  newSize,
                                  newSampleCount,
                                  newTextureFilter,
                                  newMaxAnisotropy,
                                  newVerticalSync,
                                  newDepth,
                                  newStencil,
                                  newDebugRenderer);

            if (!wglMakeCurrent(deviceContext, nullptr))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to unset OpenGL context");

            running = true;
            renderThread = std::thread(&OGLRenderDeviceWin::main, this);
        }

        void OGLRenderDeviceWin::present()
        {
            if (!SwapBuffers(deviceContext))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to swap buffers");
        }

        void OGLRenderDeviceWin::main()
        {
            setCurrentThreadName("Render");

            if (!wglMakeCurrent(deviceContext, renderContext))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to set current OpenGL context");

            while (running)
            {
                try
                {
                    process();
                }
                catch (const std::exception& e)
                {
                    engine->log(Log::Level::ERR) << e.what();
                }
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
