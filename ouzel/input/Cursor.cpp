// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Cursor.hpp"
#include "InputManager.hpp"
#include "CursorResource.hpp"
#include "core/Engine.hpp"
#include "graphics/ImageDataSTB.hpp"

namespace ouzel
{
    namespace input
    {
        Cursor::Cursor()
        {
            resource = engine->getInputManager()->createCursorResource();
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
            if (engine && resource) engine->getInputManager()->deleteCursorResource(resource);
        }

        void Cursor::init(SystemCursor systemCursor)
        {
            engine->executeOnMainThread(std::bind(static_cast<void(CursorResource::*)(SystemCursor)>(&CursorResource::init),
                                                  resource,
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
            engine->executeOnMainThread(std::bind(static_cast<void(CursorResource::*)(const std::vector<uint8_t>&, const Size2&, graphics::PixelFormat, const Vector2&)>(&CursorResource::init),
                                                  resource,
                                                  data,
                                                  size,
                                                  pixelFormat,
                                                  hotSpot));
        }
    } // namespace input
} // namespace ouzel
