// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <unordered_map>
#import <Carbon/Carbon.h>
#import <GameController/GameController.h>
#import <objc/message.h>
#include "input/macos/InputManagerMacOS.hpp"
#include "input/macos/NativeCursorMacOS.hpp"
#include "core/macos/NativeWindowMacOS.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace input
    {
        InputManagerMacOS::InputManagerMacOS()
        {
            currentCursor = defaultCursor = [NSCursor arrowCursor];

            unsigned char* data = emptyCursorData;

            NSImage* image = [[NSImage alloc] initWithSize:NSMakeSize(1, 1)];
            NSBitmapImageRep* rep = [[NSBitmapImageRep alloc]
                                     initWithBitmapDataPlanes:&data
                                     pixelsWide:1
                                     pixelsHigh:1
                                     bitsPerSample:8
                                     samplesPerPixel:4
                                     hasAlpha:YES
                                     isPlanar:NO
                                     colorSpaceName:NSDeviceRGBColorSpace
                                     bitmapFormat:NSAlphaNonpremultipliedBitmapFormat
                                     bytesPerRow:4
                                     bitsPerPixel:32];

            [image addRepresentation:rep];
            emptyCursor = [[NSCursor alloc] initWithImage:image hotSpot:NSMakePoint(0, 0)];

            [image release];
            [rep release];
        }

        InputManagerMacOS::~InputManagerMacOS()
        {
            resourceDeleteSet.clear();
            resources.clear();
        }

        void InputManagerMacOS::activateNativeCursor(NativeCursor* resource)
        {
            InputManager::activateNativeCursor(resource);

            NativeCursorMacOS* cursorMacOS = static_cast<NativeCursorMacOS*>(resource);

            if (cursorMacOS)
            {
                currentCursor = cursorMacOS->getNativeCursor();
                if (!currentCursor) currentCursor = emptyCursor;
            }
            else
                currentCursor = defaultCursor;

            NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(engine->getWindow()->getNativeWindow());
            [windowMacOS->getNativeWindow() invalidateCursorRectsForView:windowMacOS->getNativeView()];
        }

        NativeCursor* InputManagerMacOS::createNativeCursor()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            std::unique_ptr<NativeCursorMacOS> cursorResource(new NativeCursorMacOS(*this));
            NativeCursor* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void InputManagerMacOS::setCursorVisible(bool visible)
        {
            /*if (visible != cursorVisible)
            {
                cursorVisible = visible;

                engine->executeOnMainThread([this, visible] {
                    if (visible)
                        [currentCursor set];
                    else
                        [emptyCursor set];
                });
            }*/
        }

        bool InputManagerMacOS::isCursorVisible() const
        {
            //return cursorVisible;
            return true;
        }
    } // namespace input
} // namespace ouzel
