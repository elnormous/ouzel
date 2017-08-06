// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Noncopyable.hpp"

namespace ouzel
{
    namespace graphics
    {
        class ResourceInterface: public Noncopyable
        {
        public:
            ResourceInterface() {}
            virtual ~ResourceInterface() {}
        };
    } // namespace graphics
} // namespace ouzel
