// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "stb_vorbis.c"
#include "SoundDataVorbis.h"
#include "StreamVorbis.h"
#include "core/Engine.h"
#include "files/FileSystem.h"
#include "utils/Log.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace audio
    {
        SoundDataVorbis::SoundDataVorbis()
        {
        }

        SoundDataVorbis::~SoundDataVorbis()
        {
        }

        bool SoundDataVorbis::init(const std::string& newFilename)
        {
            filename = newFilename;

            std::vector<uint8_t> newData;
            if (!sharedEngine->getFileSystem()->readFile(newFilename, newData))
            {
                return false;
            }

            return init(newData);
        }

        bool SoundDataVorbis::init(const std::vector<uint8_t>& newData)
        {
            return true;
        }

        std::unique_ptr<Stream> SoundDataVorbis::createStream()
        {
            std::unique_ptr<Stream> stream(new StreamVorbis());

            return stream;
        }

        std::vector<uint8_t> SoundDataVorbis::getData(Stream* stream, uint32_t size)
        {
            if (!stream)
            {
                return data;
            }
            else
            {
                StreamVorbis* streamVorbis = static_cast<StreamVorbis*>(stream);

                std::vector<uint8_t> result;

                return result;
            }
        }
    } // namespace audio
} // namespace ouzel
