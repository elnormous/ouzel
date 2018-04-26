// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CursorResource.hpp"
#include "input/Input.hpp"

namespace ouzel
{
    namespace input
    {
        CursorResource::CursorResource(Input* initInput):
            input(initInput)
        {
        }

        CursorResource::~CursorResource()
        {
        }

        bool CursorResource::init(SystemCursor newSystemCursor)
        {
            systemCursor = newSystemCursor;

            return true;
        }

        bool CursorResource::init(const std::vector<uint8_t>& newData,
                                  const Size2& newSize,
                                  graphics::PixelFormat newPixelFormat,
                                  const Vector2& newHotSpot)
        {
            data = newData;
            size = newSize;
            pixelFormat = newPixelFormat;
            hotSpot = newHotSpot;

            return true;
        }


        void CursorResource::reactivate()
        {
            if (input->currentCursorResource == this)
                input->activateCursorResource(this);
        }
    } // namespace input
} // namespace ouzel
