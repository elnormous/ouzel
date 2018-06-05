// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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

        bool Cursor::init(SystemCursor systemCursor)
        {
            engine->executeOnMainThread(std::bind(static_cast<bool(CursorResource::*)(SystemCursor)>(&CursorResource::init),
                                                  resource,
                                                  systemCursor));

            return true;
        }

        bool Cursor::init(const std::string& filename, const Vector2& hotSpot)
        {
            // TODO: load with asset loader
            graphics::ImageDataSTB image;
            if (!image.init(filename))
                return false;

            return init(image.getData(),
                        image.getSize(),
                        image.getPixelFormat(),
                        hotSpot);
        }

        bool Cursor::init(const std::vector<uint8_t>& data,
                          const Size2& size,
                          graphics::PixelFormat pixelFormat,
                          const Vector2& hotSpot)
        {
            engine->executeOnMainThread(std::bind(static_cast<bool(CursorResource::*)(const std::vector<uint8_t>&, const Size2&, graphics::PixelFormat, const Vector2&)>(&CursorResource::init),
                                                  resource,
                                                  data,
                                                  size,
                                                  pixelFormat,
                                                  hotSpot));

            return true;
        }
    } // namespace input
} // namespace ouzel
