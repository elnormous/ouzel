// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <unordered_map>
#include <Windows.h>
#include <WbemIdl.h>
#include <OleAuto.h>
#include "InputManagerWin.hpp"
#include "NativeCursorWin.hpp"
#include "GamepadDeviceDI.hpp"
#include "GamepadDeviceXI.hpp"
#include "core/Engine.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace input
    {
        InputManagerWin::InputManagerWin()
        {
            currentCursor = defaultCursor = LoadCursor(nullptr, IDC_ARROW);
        }

        InputManagerWin::~InputManagerWin()
        {
            resourceDeleteSet.clear();
            resources.clear();
        }

        void InputManagerWin::activateNativeCursor(NativeCursor* resource)
        {
            InputManager::activateNativeCursor(resource);

            NativeCursorWin* cursorWin = static_cast<NativeCursorWin*>(resource);

            if (cursorWin)
                currentCursor = cursorWin->getNativeCursor();
            else
                currentCursor = defaultCursor;

            updateCursor();
        }

        NativeCursor* InputManagerWin::createNativeCursor()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            std::unique_ptr<NativeCursorWin> cursorResource(new NativeCursorWin(*this));
            NativeCursor* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void InputManagerWin::setCursorVisible(bool visible)
        {
            cursorVisible = visible;

            engine->executeOnMainThread([this, visible] {
                if (visible)
                    SetCursor(currentCursor);
                else
                    SetCursor(nullptr);
            });
        }

        bool InputManagerWin::isCursorVisible() const
        {
            return cursorVisible;
        }

        void InputManagerWin::setCursorLocked(bool locked)
        {
            engine->executeOnMainThread([locked] {
                if (locked)
                {
                    HWND nativeWindow = static_cast<NativeWindowWin*>(engine->getWindow()->getNativeWindow())->getNativeWindow();

                    RECT rect;
                    GetWindowRect(nativeWindow, &rect);

                    LONG centerX = (rect.left + rect.right) / 2;
                    LONG centerY = (rect.top + rect.bottom) / 2;

                    rect.left = centerX;
                    rect.right = centerX + 1;
                    rect.top = centerY;
                    rect.bottom = centerY + 1;

                    if (!ClipCursor(&rect))
                        throw SystemError("Failed to grab pointer");
                }
                else
                    ClipCursor(nullptr);
            });
            cursorLocked = locked;
        }

        bool InputManagerWin::isCursorLocked() const
        {
            return cursorLocked;
        }

        void InputManagerWin::updateCursor()
        {
            if (cursorVisible)
                SetCursor(currentCursor);
            else
                SetCursor(nullptr);
        }
    } // namespace input
} // namespace ouzel
