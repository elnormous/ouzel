// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <mutex>
#include "utils/Noncopyable.h"
#include "audio/Resource.h"

namespace ouzel
{
    namespace audio
    {
        class SoundDataResource: public Resource, public Noncopyable
        {
        public:
        };
    } // namespace audio
} // namespace ouzel
