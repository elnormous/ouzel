// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundDS.h"
#include "AudioDS.h"
#include "audio/SoundData.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace audio
    {
        SoundDS::SoundDS()
        {
        }

        SoundDS::~SoundDS()
        {
            if (buffer3D) buffer3D->Release();
            if (buffer) buffer->Release();
        }

        bool SoundDS::update()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty & DIRTY_SOUND_DATA)
            {
            }

            dirty = 0;

            return true;
        }
    } // namespace audio
} // namespace ouzel
