

#include <windowsx.h>

#include "Win32View.h"
#include "../Renderer.h"
#include "../Engine.h"
#include "../EventHander.h"


namespace ouzel
{
    LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
}


void ouzel::View::createNativeView()
{
}

void ouzel::View::destroyNativeView()
{
}

ouzel::Win32View::Win32View(const Size2& size, Renderer* renderer) : View(size, renderer)
{
    createNativeView();
}

void ouzel::Win32View::createNativeView()
{
    HINSTANCE instance = GetModuleHandle(NULL);
    
    WNDCLASSEXA wc;
    memset(&wc, 0, sizeof(WNDCLASSEXA));
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = "OuzelWindow";
    RegisterClassExA(&wc);
    
    DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT windowRect = {0, 0, (LONG) _size.width, (LONG) _size.height};
    AdjustWindowRect(&windowRect, style, FALSE);
    
    _window = CreateWindowExA(
        NULL,
        "OuzelWindow",
        "Ouzel",
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        instance,
        NULL);
    SetWindowLongPtrA(_window, GWLP_USERDATA, (LONG_PTR) this);
    
    ShowWindow(_window, SW_SHOW);
}

void ouzel::Win32View::destroyNativeView()
{
    DestroyWindow(_window);
}

bool ouzel::Win32View::processEvents()
{
    bool run = true;
    
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        if (msg.message == WM_QUIT)
        {
            run = false;
            break;
        }
    }
    
    return run;
}


static ouzel::Win32View* getView(HWND window)
{
    return (ouzel::Win32View*) GetWindowLongPtrA(window, GWLP_USERDATA);
}

namespace ouzel
{
    void mouseButtonEvent(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        bool isDown = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
        int button = 0;
        if(msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
        {
            button = 1;
        }
        else if(msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
        {
            button = 2;
        }
        Win32View* view = getView(window);
        Vector2 pos((float) GET_X_LPARAM(lParam), (float) GET_Y_LPARAM(lParam));
        
        Event event;
        memset(&event, 0, sizeof(event));
        event.type = isDown ? EventType::EVENT_TYPE_MOUSE_DOWN : EventType::EVENT_TYPE_MOUSE_UP;
        event.mouseButton = button;
        event.mousePosition = view->getRenderer()->absoluteToWorldLocation(pos);
        if(wParam & MK_SHIFT)
        {
            event.shiftDown = true;
        }
        if(wParam & MK_CONTROL)
        {
            event.controlDown = true;
        }
        view->getRenderer()->getEngine()->handleEvent(event);
    }
}

LRESULT CALLBACK ouzel::WindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_KEYUP:
        break;
    case WM_KEYDOWN:
        break;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        mouseButtonEvent(window, msg, wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(window, msg, wParam, lParam);
}

