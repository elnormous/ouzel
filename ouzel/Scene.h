// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class Scene: public Noncopyable, public ReferenceCounted
    {
    public:
        Scene();
        virtual ~Scene();
    };
}
