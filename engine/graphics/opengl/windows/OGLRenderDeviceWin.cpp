// Ouzel by Elviss Strazdins

#include "../../../core/Setup.h"

#if defined(_WIN32) && OUZEL_COMPILE_OPENGL

#include <string_view>
#include <system_error>
#include "GL/glcorearb.h"
#include "GL/glext.h"
#include "GL/wglext.h"
#include "OGLRenderDeviceWin.hpp"
#include "../../../core/Engine.hpp"
#include "../../../core/Window.hpp"
#include "../../../core/windows/NativeWindowWin.hpp"
#include "../../../utils/Log.hpp"
#include "../../../platform/winapi/Library.hpp"

namespace ouzel::graphics::opengl::windows
{
    namespace
    {
        LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return DefWindowProc(window, message, wParam, lParam);
        }

        class TempContext final
        {
        public:
            TempContext()
            {
                HINSTANCE instance = GetModuleHandleW(nullptr);
                if (!instance)
                    throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get module handle"};

                WNDCLASSW wc;
                wc.style = CS_OWNDC;
                wc.lpfnWndProc = windowProc;
                wc.cbClsExtra = 0;
                wc.cbWndExtra = 0;
                wc.hInstance = instance;
                wc.hIcon = nullptr;
                wc.hCursor = nullptr;
                wc.hbrBackground = nullptr;
                wc.lpszMenuName = nullptr;
                wc.lpszClassName = L"TempWindow";

                windowClass = RegisterClassW(&wc);
                if (!windowClass)
                    throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to register window class"};

                window = CreateWindowW(reinterpret_cast<LPCWSTR>(MAKEINTATOM(windowClass)), L"TempWindow", 0,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       CW_USEDEFAULT, CW_USEDEFAULT,
                                       0, 0, instance, 0);
                if (!window)
                    throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to create window"};

                deviceContext = GetDC(window);
                if (!deviceContext)
                    throw std::runtime_error{"Failed to get device context"};

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

                const auto pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
                if (!pixelFormat)
                    throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to choose pixel format"};

                if (!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc))
                    throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to set pixel format"};

                renderContext = wglCreateContext(deviceContext);
                if (!renderContext)
                    throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to create OpenGL rendering context"};

                if (!wglMakeCurrent(deviceContext, renderContext))
                    throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to set OpenGL rendering context"};
            }

            ~TempContext()
            {
                if (renderContext)
                {
                    if (wglGetCurrentContext() == renderContext)
                        wglMakeCurrent(deviceContext, nullptr);
                    wglDeleteContext(renderContext);
                }

                if (window)
                {
                    if (deviceContext)
                        ReleaseDC(window, deviceContext);

                    DestroyWindow(window);
                }

                if (windowClass)
                    UnregisterClassW(reinterpret_cast<LPCWSTR>(MAKEINTATOM(windowClass)), GetModuleHandleW(nullptr));
            }

            TempContext(const TempContext&) = delete;
            TempContext& operator=(const TempContext&) = delete;
            TempContext(TempContext&&) = delete;
            TempContext& operator=(TempContext&&) = delete;

        private:
            ATOM windowClass = 0;
            HWND window = nullptr;
            HDC deviceContext = nullptr;
            HGLRC renderContext = nullptr;
        };

        ApiVersion getVersion()
        {
            platform::winapi::Library library{"opengl32.dll"};

            const auto glGetStringProc = reinterpret_cast<PFNGLGETSTRINGPROC>(library.getProcAddress("glGetString"));

            const auto versionPtr = glGetStringProc(GL_VERSION);

            if (!versionPtr)
                throw std::runtime_error{"Failed to get OpenGL version"};

            const std::string_view versionStr{reinterpret_cast<const char*>(versionPtr)};
            ApiVersion version;
            std::size_t part = 0;

            for (const auto c : versionStr)
                if (c != '.')
                {
                    if (c < '0' || c > '9')
                        throw std::runtime_error{"Invalid OpenGL version"};

                    version.v[part] = static_cast<std::uint16_t>(version.v[part] * 10 + c - '0');
                }
                else if (++part > 1)
                    break;

            return version;
        }
    }

    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& initWindow):
        opengl::RenderDevice{settings, initWindow}
    {
        embedded = false;

        const TempContext tempContext;

        const auto& nativeWindow = window.getNativeWindow();

        deviceContext = GetDC(nativeWindow.getNativeWindow());
        if (!deviceContext)
            throw std::runtime_error{"Failed to get window's device context"};

        std::vector<std::string> extensions;

        if (const auto wglGetExtensionsStringProc = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB")))
        {
            if (const char* extensionsPtr = wglGetExtensionsStringProc(deviceContext))
                extensions = explodeString(extensionsPtr, ' ');

            log(Log::Level::all) << "Supported WGL extensions: " << extensions;
        }

        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatProc = nullptr;
        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsProc = nullptr;

        for (const auto& extension : extensions)
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
        pixelFormatDesc.cDepthBits = settings.depth ? 24 : 0;
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
                WGL_DEPTH_BITS_ARB, settings.depth ? 24 : 0,
                WGL_STENCIL_BITS_ARB, settings.stencil ? 8 : 0,
                WGL_SAMPLE_BUFFERS_ARB, settings.sampleCount > 0 ? 1 : 0,
                WGL_SAMPLES_ARB, static_cast<int>(settings.sampleCount),
                WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, settings.srgb ? 1 : 0,
                0,
            };

            UINT numFormats;

            if (!wglChoosePixelFormatProc(deviceContext, attributeList, nullptr, 1, &pixelFormat, &numFormats))
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to choose pixel format"};
        }
        else
        {
            pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatDesc);
            if (!pixelFormat)
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to choose pixel format"};
        }

        if (!SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDesc))
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to set pixel format"};

        if (wglCreateContextAttribsProc)
        {
            for (const auto openGlVersion : {4, 3, 2})
            {
                const int contextAttribs[] = {
                    WGL_CONTEXT_MAJOR_VERSION_ARB, openGlVersion,
                    WGL_CONTEXT_MINOR_VERSION_ARB, 0,
                    WGL_CONTEXT_FLAGS_ARB, settings.debugRenderer ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
                    0
                };

                renderContext = wglCreateContextAttribsProc(deviceContext, 0, contextAttribs);

                if (renderContext)
                {
                    log(Log::Level::info) << "OpenGL " << openGlVersion << " context created";
                    break;
                }
            }
        }
        else
            renderContext = wglCreateContext(deviceContext);

        if (!renderContext)
            throw std::runtime_error{"Failed to create OpenGL rendering context"};

        if (!wglMakeCurrent(deviceContext, renderContext))
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to set OpenGL rendering context"};

        apiVersion = getVersion();

        if (apiVersion.v[0] < 2 || apiVersion.v[0] > 4)
            throw std::runtime_error{"Unsupported OpenGL version"};

        init(static_cast<GLsizei>(window.getResolution().v[0]),
             static_cast<GLsizei>(window.getResolution().v[1]));

        if (!wglMakeCurrent(deviceContext, nullptr))
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to unset OpenGL rendering context"};
    }

    RenderDevice::~RenderDevice()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));

        if (renderThread.isJoinable()) renderThread.join();

        if (renderContext)
        {
            if (wglGetCurrentContext() == renderContext)
                wglMakeCurrent(deviceContext, nullptr);
            wglDeleteContext(renderContext);
        }

        if (deviceContext)
        {
            const auto& nativeWindow = window.getNativeWindow();
            ReleaseDC(nativeWindow.getNativeWindow(), deviceContext);
        }
    }
    
    void RenderDevice::start()
    {
        running = true;
        renderThread = thread::Thread{&RenderDevice::renderMain, this};
    }

    void RenderDevice::present()
    {
        if (!SwapBuffers(deviceContext))
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to swap buffers"};
    }

    void RenderDevice::renderMain()
    {
        if (!wglMakeCurrent(deviceContext, renderContext))
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to set OpenGL rendering context"};

        while (running)
        {
            try
            {
                process();
            }
            catch (const std::exception& e)
            {
                log(Log::Level::error) << e.what();
            }
        }

        if (!wglMakeCurrent(deviceContext, nullptr))
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to unset OpenGL rendering context"};
    }
}

#endif
