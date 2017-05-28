// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Noncopyable.h"

namespace ouzel
{
    class CursorResource;

    class Cursor: public Noncopyable
    {
    public:
        Cursor();
        virtual ~Cursor();

    private:
        CursorResource* resource = nullptr;
    };
}
