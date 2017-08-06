// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioEmpty.hpp"

namespace ouzel
{
    namespace audio
    {
        AudioEmpty::AudioEmpty():
            Audio(Driver::EMPTY)
        {
        }
    } // namespace audio
} // namespace ouzel
