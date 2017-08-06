// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Noncopyable.hpp"

namespace ouzel
{
    namespace audio
    {
        class Stream: public Noncopyable
        {
        public:
        	virtual ~Stream();

            virtual void reset() = 0;
        };
    } // namespace audio
} // namespace ouzel
