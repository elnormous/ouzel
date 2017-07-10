// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundData.h"
#include "core/Application.h"
#include "files/FileSystem.h"
#include "utils/Log.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace audio
    {
        SoundData::SoundData()
        {
        }

        SoundData::~SoundData()
        {
        }

        bool SoundData::init(const std::string&)
        {
            return true;
        }

        bool SoundData::init(const std::vector<uint8_t>&)
        {
            return true;
        }
    } // namespace audio
} // namespace ouzel
