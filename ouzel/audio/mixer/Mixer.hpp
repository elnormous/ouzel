// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_MIXER_HPP
#define OUZEL_AUDIO_MIXER_MIXER_HPP

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <set>
#include <vector>
#include "audio/mixer/Commands.hpp"
#include "audio/mixer/Object.hpp"
#include "audio/mixer/Processor.hpp"

namespace ouzel
{
    namespace audio
    {
        /*
            Channel configurations:
            Mono: mono (0)
            Stereo: left (0), right (1)
            Quad: left (0), right (1), surround left (2), surround right (3)
            5.1: left (0), right (1), center (2), LFE (3) surround left (4), surround right (5)
        */
        class Mixer
        {
        public:
            Mixer() {}

            Mixer(const Mixer&) = delete;
            Mixer& operator=(const Mixer&) = delete;

            Mixer(Mixer&&) = delete;
            Mixer& operator=(Mixer&&) = delete;

            void addCommand(std::unique_ptr<Command>&& command);
            
            void process();
            void getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples);

            uintptr_t getObjectId()
            {
                auto i = deletedObjectIds.begin();

                if (i == deletedObjectIds.end())
                    return ++lastObjectId; // zero is reserved for null node
                else
                {
                    uintptr_t objectId = *i;
                    deletedObjectIds.erase(i);
                    return objectId;
                }
            }

            void deleteObjectId(uintptr_t objectId)
            {
                deletedObjectIds.insert(objectId);
            }

        private:
            uintptr_t lastObjectId = 0;
            std::set<uintptr_t> deletedObjectIds;

            std::vector<std::unique_ptr<Object>> objects;

            Bus* masterBus = nullptr;
            std::mutex commandMutex;
            std::condition_variable commandConditionVariable;
            std::queue<std::unique_ptr<Command>> commandQueue;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_MIXER_HPP
