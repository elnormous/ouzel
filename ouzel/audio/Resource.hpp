// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Noncopyable.hpp"

namespace ouzel
{
    namespace audio
    {
        class Resource: public Noncopyable
        {
        public:
            Resource() {}
            virtual ~Resource() {}
        };
    } // namespace audio
} // namespace ouzel
