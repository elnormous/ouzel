// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "audio/Audio.h"

namespace ouzel
{
    class Engine;

    namespace audio
    {
        class AudioEmpty: public Audio
        {
            friend Engine;
        public:
            virtual std::shared_ptr<Sound> createSound() override;

        protected:
            AudioEmpty();
        };
    } // namespace audio
} // namespace ouzel
