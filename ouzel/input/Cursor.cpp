// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Cursor.hpp"
#include "InputManager.hpp"
#include "NativeCursor.hpp"
#include "core/Engine.hpp"
#include "graphics/ImageDataSTB.hpp"

namespace ouzel
{
    namespace input
    {
        Cursor::Cursor()
        {
            nativeCursor = engine->getInputManager()->createNativeCursor();
        }

        Cursor::Cursor(SystemCursor systemCursor):
            Cursor()
        {
            init(systemCursor);
        }

        Cursor::Cursor(const std::string& filename, const Vector2& hotSpot):
            Cursor()
        {
            init(filename, hotSpot);
        }

        Cursor::~Cursor()
        {
            if (engine && nativeCursor) engine->getInputManager()->deleteNativeCursor(nativeCursor);
        }

        void Cursor::init(SystemCursor systemCursor)
        {
            engine->executeOnMainThread(std::bind(static_cast<void(NativeCursor::*)(SystemCursor)>(&NativeCursor::init),
                                                  nativeCursor,
                                                  systemCursor));
        }

        void Cursor::init(const std::string& filename, const Vector2& hotSpot)
        {
            // TODO: load with asset loader
            graphics::ImageDataSTB image;
            image.init(filename);

            init(image.getData(),
                 image.getSize(),
                 image.getPixelFormat(),
                 hotSpot);
        }

        void Cursor::init(const std::vector<uint8_t>& data,
                          const Size2& size,
                          graphics::PixelFormat pixelFormat,
                          const Vector2& hotSpot)
        {
            engine->executeOnMainThread(std::bind(static_cast<void(NativeCursor::*)(const std::vector<uint8_t>&, const Size2&, graphics::PixelFormat, const Vector2&)>(&NativeCursor::init),
                                                  nativeCursor,
                                                  data,
                                                  size,
                                                  pixelFormat,
                                                  hotSpot));
        }
    } // namespace input
} // namespace ouzel
