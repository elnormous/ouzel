// Ouzel by Elviss Strazdins

#include "../Setup.h"

#include <stdexcept>
#include <X11/extensions/Xrandr.h> 
#include "NativeWindowLinux.hpp"
#include "EngineLinux.hpp"

#if OUZEL_SUPPORTS_X11
namespace
{
    constexpr long NET_WM_STATE_REMOVE = 0L;
    constexpr long NET_WM_STATE_ADD = 1L;
    constexpr long NET_WM_STATE_TOGGLE = 2L;
}
#endif

namespace ouzel::core::linux
{
    NativeWindow::NativeWindow(const math::Size<std::uint32_t, 2>& newSize,
                               bool newResizable,
                               bool newFullscreen,
                               bool newExclusiveFullscreen,
                               const std::string& newTitle):
        core::NativeWindow{
            newSize,
            newResizable,
            newFullscreen,
            newExclusiveFullscreen,
            newTitle,
            true
        }
    {
#if OUZEL_SUPPORTS_X11
        const auto engineLinux = static_cast<Engine*>(engine);
        display = engineLinux->getDisplay();

        const auto screen = XDefaultScreenOfDisplay(display);
        screenNumber = XScreenNumberOfScreen(screen);

        const auto rootWindow = RootWindow(display, screenNumber);


        int monitorCount;
        using FreeMonitorsFunction = void(*)(XRRMonitorInfo*);
        std::unique_ptr<XRRMonitorInfo, FreeMonitorsFunction> monitors{
            XRRGetMonitors(display, rootWindow, True, &monitorCount),
            &XRRFreeMonitors
        };

        XRRMonitorInfo* primaryMonitor = (monitorCount > 0) ? &monitors.get()[0] : nullptr;
        for (int i = 0; i < monitorCount; ++i)
            if (monitors.get()[i].primary)
            {
                primaryMonitor = &monitors.get()[i];
                break;
            }

        constexpr std::uint32_t defaultWidth = 800U;
        constexpr std::uint32_t defaultHeight = 600U;

        if (size.v[0] <= 0.0F) size.v[0] = primaryMonitor ? static_cast<std::uint32_t>(primaryMonitor->width * 0.8F) : defaultWidth;
        if (size.v[1] <= 0.0F) size.v[1] = primaryMonitor ? static_cast<std::uint32_t>(primaryMonitor->height * 0.8F) : defaultHeight;

        resolution = size;

        const int x = primaryMonitor ? primaryMonitor->x + primaryMonitor->width / 2 - static_cast<int>(size.v[0] / 2) : 0;
        const int y = primaryMonitor ? primaryMonitor->y + primaryMonitor->height / 2 - static_cast<int>(size.v[1] / 2) : 0;

        XSetWindowAttributes attributes;
        attributes.background_pixel = XWhitePixel(display, screenNumber);
        attributes.border_pixel = 0;
        attributes.event_mask = FocusChangeMask | KeyPressMask | KeyRelease | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

        window = XCreateWindow(display, rootWindow, x, y,
                               static_cast<unsigned int>(size.v[0]), static_cast<unsigned int>(size.v[1]),
                               0, DefaultDepth(display, screenNumber), InputOutput, DefaultVisual(display, screenNumber),
                               CWBorderPixel | CWBackPixel | CWEventMask, &attributes);

        if (XSetStandardProperties(display, window, title.c_str(), title.c_str(), None, nullptr, 0, nullptr) == 0)
            throw std::system_error{getLastError(), getErrorCategory(), "Failed to set window properties"};

        if (!resizable)
        {
            XSizeHints sizeHints;
            sizeHints.flags = PPosition | PMinSize | PMaxSize;
            sizeHints.x = x;
            sizeHints.y = y;
            sizeHints.min_width = static_cast<int>(size.v[0]);
            sizeHints.max_width = static_cast<int>(size.v[0]);
            sizeHints.min_height = static_cast<int>(size.v[1]);
            sizeHints.max_height = static_cast<int>(size.v[1]);
            XSetWMNormalHints(display, window, &sizeHints);
        }
        else
        {
            XSizeHints sizeHints;
            sizeHints.flags = PPosition;
            sizeHints.x = x;
            sizeHints.y = y;
            XSetWMNormalHints(display, window, &sizeHints);
        }

        if (XMapWindow(display, window) == 0)
            throw std::system_error{getLastError(), getErrorCategory(), "Failed to map window"};

        protocolsAtom = XInternAtom(display, "WM_PROTOCOLS", False);
        deleteAtom = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &deleteAtom, 1);
        stateAtom = XInternAtom(display, "_NET_WM_STATE", False);
        if (!stateAtom)
            throw std::system_error{getLastError(), getErrorCategory(), "State atom is null"};
        stateFullscreenAtom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
        if (!stateFullscreenAtom)
            throw std::system_error{getLastError(), getErrorCategory(), "Fullscreen state atom is null"};
        activateWindowAtom = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);

        if (fullscreen)
        {
            XEvent event;
            event.type = ClientMessage;
            event.xclient.window = window;
            event.xclient.message_type = stateAtom;
            event.xclient.format = 32;
            event.xclient.data.l[0] = NET_WM_STATE_ADD;
            event.xclient.data.l[1] = stateFullscreenAtom;
            event.xclient.data.l[2] = 0; // no second property to toggle
            event.xclient.data.l[3] = 1; // source indication: application
            event.xclient.data.l[4] = 0; // unused

            if (XSendEvent(display, DefaultRootWindow(display), 0,
                           SubstructureNotifyMask | SubstructureRedirectMask, &event) == 0)
                throw std::system_error{getLastError(), getErrorCategory(), "Failed to send X11 fullscreen message"};
        }
#elif OUZEL_SUPPORTS_DISPMANX
        const auto engineLinux = static_cast<Engine*>(engine);
        auto display = engineLinux->getDisplay();

        DISPMANX_MODEINFO_T modeInfo;
        const std::int32_t success = vc_dispmanx_display_get_info(display, &modeInfo);

        if (success < 0)
            throw std::runtime_error{"Failed to get display size"};

        VC_RECT_T dstRect;
        dstRect.x = 0;
        dstRect.y = 0;
        dstRect.width = modeInfo.width;
        dstRect.height = modeInfo.height;

        VC_RECT_T srcRect;
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.width = modeInfo.width;
        srcRect.height = modeInfo.height;

        dispmanUpdate = vc_dispmanx_update_start(0);

        if (dispmanUpdate == DISPMANX_NO_HANDLE)
            throw std::runtime_error{"Failed to start display update"};

        dispmanElement = vc_dispmanx_element_add(dispmanUpdate, display,
                                                 0, &dstRect, 0,
                                                 &srcRect, DISPMANX_PROTECTION_NONE,
                                                 0, 0, DISPMANX_NO_ROTATE);

        if (dispmanElement == DISPMANX_NO_HANDLE)
            throw std::runtime_error{"Failed to add display element"};

        window.element = dispmanElement;
        window.width = modeInfo.width;
        window.height = modeInfo.height;
        vc_dispmanx_update_submit_sync(dispmanUpdate);

        size.v[0] = static_cast<std::uint32_t>(modeInfo.width);
        size.v[1] = static_cast<std::uint32_t>(modeInfo.height);
        resolution = size;
#endif
    }

    NativeWindow::~NativeWindow()
    {
#if OUZEL_SUPPORTS_X11
        if (display && window)
            XDestroyWindow(display, window);
#elif OUZEL_SUPPORTS_DISPMANX
        if (dispmanUpdate != DISPMANX_NO_HANDLE && dispmanElement != DISPMANX_NO_HANDLE)
            vc_dispmanx_element_remove(dispmanUpdate, dispmanElement);
#endif
    }

    void NativeWindow::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::changeSize:
                setSize(command.size);
                break;
            case Command::Type::changeFullscreen:
                setFullscreen(command.fullscreen);
                break;
            case Command::Type::close:
                close();
                break;
            case Command::Type::setTitle:
                setTitle(command.title);
                break;
            case Command::Type::bringToFront:
                bringToFront();
                break;
            case Command::Type::show:
                show();
                break;
            case Command::Type::hide:
                hide();
                break;
            case Command::Type::minimize:
                minimize();
                break;
            case Command::Type::maximize:
                maximize();
                break;
            case Command::Type::restore:
                restore();
                break;
            default:
                throw std::runtime_error{"Invalid command"};
        }
    }

    void NativeWindow::close()
    {
#if OUZEL_SUPPORTS_X11
        if (!protocolsAtom || !deleteAtom) return;

        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = window;
        event.xclient.message_type = protocolsAtom;
        event.xclient.format = 32; // data is set as 32-bit integers
        event.xclient.data.l[0] = deleteAtom;
        event.xclient.data.l[1] = CurrentTime;
        event.xclient.data.l[2] = 0; // unused
        event.xclient.data.l[3] = 0; // unused
        event.xclient.data.l[4] = 0; // unused
        if (XSendEvent(display, window, False, NoEventMask, &event) == 0)
            throw std::system_error{getLastError(), getErrorCategory(), "Failed to send X11 delete message"};
#endif
    }

    void NativeWindow::setSize(const math::Size<std::uint32_t, 2>& newSize)
    {
        size = newSize;

#if OUZEL_SUPPORTS_X11
        XWindowChanges changes;
        changes.width = static_cast<int>(size.v[0]);
        changes.height = static_cast<int>(size.v[1]);
        XConfigureWindow(display, window, CWWidth | CWHeight, &changes);

        if (!resizable)
        {
            XSizeHints sizeHints;
            sizeHints.flags = PMinSize | PMaxSize;
            sizeHints.min_width = static_cast<int>(size.v[0]);
            sizeHints.max_width = static_cast<int>(size.v[0]);
            sizeHints.min_height = static_cast<int>(size.v[1]);
            sizeHints.max_height = static_cast<int>(size.v[1]);
            XSetWMNormalHints(display, window, &sizeHints);
        }

        resolution = size;

        Event resolutionChangeEvent(Event::Type::resolutionChange);
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
#endif
    }

    void NativeWindow::setFullscreen(bool newFullscreen)
    {
#if OUZEL_SUPPORTS_X11
        if (fullscreen != newFullscreen)
        {
            XEvent event;
            event.type = ClientMessage;
            event.xclient.window = window;
            event.xclient.message_type = stateAtom;
            event.xclient.format = 32;
            event.xclient.data.l[0] = newFullscreen ? NET_WM_STATE_ADD : NET_WM_STATE_REMOVE;
            event.xclient.data.l[1] = stateFullscreenAtom;
            event.xclient.data.l[2] = 0; // no second property to toggle
            event.xclient.data.l[3] = 1; // source indication: application
            event.xclient.data.l[4] = 0; // unused

            if (XSendEvent(display, DefaultRootWindow(display), 0,
                           SubstructureNotifyMask | SubstructureRedirectMask, &event) == 0)
                throw std::system_error{getLastError(), getErrorCategory(), "Failed to send X11 fullscreen message"};
        }
#endif

        fullscreen = newFullscreen;
    }

    void NativeWindow::setTitle(const std::string& newTitle)
    {
#if OUZEL_SUPPORTS_X11
        if (title != newTitle) XStoreName(display, window, newTitle.c_str());
#endif

        title = newTitle;
    }

    void NativeWindow::bringToFront()
    {
#if OUZEL_SUPPORTS_X11
        XRaiseWindow(display, window);

        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = window;
        event.xclient.message_type = activateWindowAtom;
        event.xclient.format = 32;
        event.xclient.data.l[0] = 1; // source indication: application
        event.xclient.data.l[1] = CurrentTime;
        event.xclient.data.l[2] = 0;

        if (XSendEvent(display, DefaultRootWindow(display), 0,
                       SubstructureNotifyMask | SubstructureRedirectMask, &event) == 0)
            throw std::system_error{getLastError(), getErrorCategory(), "Failed to send X11 activate window message"};

        XFlush(display);
#endif
    }

    void NativeWindow::show()
    {
#if OUZEL_SUPPORTS_X11
        if (!isMapped())
        {
            XMapRaised(display, window);
            XFlush(display);
        }
#endif
    }

    void NativeWindow::hide()
    {
#if OUZEL_SUPPORTS_X11
        if (isMapped())
        {
            XWithdrawWindow(display, window, screenNumber);
            XFlush(display);
        }
#endif
    }

    void NativeWindow::minimize()
    {
#if OUZEL_SUPPORTS_X11
        XIconifyWindow(display, window, screenNumber);
        XFlush(display);
#endif
    }

    void NativeWindow::maximize()
    {
    }

    void NativeWindow::restore()
    {
#if OUZEL_SUPPORTS_X11
        XRaiseWindow(display, window);

        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = window;
        event.xclient.message_type = activateWindowAtom;
        event.xclient.format = 32;
        event.xclient.data.l[0] = 1; // source indication: application
        event.xclient.data.l[1] = CurrentTime;
        event.xclient.data.l[2] = 0;

        if (XSendEvent(display, DefaultRootWindow(display), 0,
                       SubstructureNotifyMask | SubstructureRedirectMask, &event) == 0)
            throw std::system_error{getLastError(), getErrorCategory(), "Failed to send X11 activate window message"};

        XFlush(display);
#endif
    }

#if OUZEL_SUPPORTS_X11
    void NativeWindow::handleFocusIn()
    {
        Event focusChangeEvent(Event::Type::focusChange);
        focusChangeEvent.focus = true;
        sendEvent(focusChangeEvent);
    }

    void NativeWindow::handleFocusOut()
    {
        Event focusChangeEvent(Event::Type::focusChange);
        focusChangeEvent.focus = false;
        sendEvent(focusChangeEvent);
    }

    void NativeWindow::handleResize(const math::Size<std::uint32_t, 2>& newSize)
    {
        size = newSize;
        resolution = size;

        Event sizeChangeEvent(Event::Type::sizeChange);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::resolutionChange);
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindow::handleMap()
    {
        sendEvent(Event(Event::Type::restore));
    }

    void NativeWindow::handleUnmap()
    {
        sendEvent(Event(Event::Type::minimize));
    }

    bool NativeWindow::isMapped() const
    {
        XWindowAttributes attributes;
        XGetWindowAttributes(display, window, &attributes);
        return attributes.map_state != IsUnmapped;
    }
#endif
}
