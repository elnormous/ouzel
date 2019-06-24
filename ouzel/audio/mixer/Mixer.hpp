// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_MIXER_HPP
#define OUZEL_AUDIO_MIXER_MIXER_HPP

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <set>
#include <thread>
#include <vector>
#include "audio/mixer/Commands.hpp"
#include "audio/mixer/Object.hpp"
#include "audio/mixer/Processor.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class RootObject final: public Object
            {
            public:
                void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples) final
                {
                    samples.resize(frames * channels);
                    std::fill(samples.begin(), samples.end(), 0.0F);

                    for (Object* child : children)
                    {
                        child->getSamples(frames, channels, sampleRate, buffer);

                        for (uint32_t channel = 0; channel < channels; ++channel)
                            for (uint32_t frame = 0; frame < frames; ++frame)
                                samples[channel * frames + frame] += buffer[channel * frames + frame];
                    }
                }

            private:
                std::vector<float> buffer;
            };

            class Mixer
            {
            public:
                class Event
                {
                public:
                    enum class Type
                    {
                        STREAM_STARTED,
                        STREAM_RESET,
                        STREAM_STOPPED,
                        STARVATION
                    };

                    Event() = default;
                    explicit Event(Type initType): type(initType) {}

                    Type type;
                    uintptr_t objectId;
                };

                Mixer(uint32_t initBufferSize,
                      uint16_t initChannels,
                      const std::function<void(const Event&)>& initCallback);

                ~Mixer();

                Mixer(const Mixer&) = delete;
                Mixer& operator=(const Mixer&) = delete;

                Mixer(Mixer&&) = delete;
                Mixer& operator=(Mixer&&) = delete;

                void process();
                void getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples);

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

                void submitCommandBuffer(CommandBuffer&& commandBuffer)
                {
                    std::unique_lock<std::mutex> lock(commandQueueMutex);
                    commandQueue.push(std::forward<CommandBuffer>(commandBuffer));
                    lock.unlock();
                }

                uintptr_t getRootObjectId() const
                {
                    return rootObjectId;
                }

            private:
                void mixerMain();

                uint32_t bufferSize;
                uint16_t channels;
                std::function<void(const Event&)> callback;

                uintptr_t lastObjectId = 0;
                std::set<uintptr_t> deletedObjectIds;

                std::vector<std::unique_ptr<Object>> objects;
                uintptr_t rootObjectId = 0;
                RootObject* rootObject = nullptr;

                Bus* masterBus = nullptr;

                class Buffer final
                {
                public:
                    Buffer(size_t size, uint16_t initChannels):
                        maxFrames(size),
                        channels(initChannels),
                        buffer(size * channels)
                    {
                    }

                    bool canRead() const { return frames > 0; }
                    bool canWrite() const { return frames < maxFrames; }

                private:
                    size_t frames = 0;
                    size_t maxFrames;
                    uint16_t channels;
                    size_t readPosition = 0;
                    size_t writePosition = 0;
                    std::vector<float> buffer;
                };

                std::thread mixerThread;
                std::mutex bufferMutex;
                std::condition_variable bufferCondition;
                Buffer buffer;

                std::queue<CommandBuffer> commandQueue;
                std::mutex commandQueueMutex;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_MIXER_HPP
