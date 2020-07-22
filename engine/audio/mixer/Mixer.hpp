// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
#include "Commands.hpp"
#include "Object.hpp"
#include "Processor.hpp"
#include "../../utils/Thread.hpp"

namespace ouzel::audio::mixer
{
    class RootObject final: public Object
    {
    public:
        void getSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate, std::vector<float>& samples) final
        {
            samples.resize(frames * channels);
            std::fill(samples.begin(), samples.end(), 0.0F);

            for (Object* child : children)
            {
                child->getSamples(frames, channels, sampleRate, buffer);

                for (std::uint32_t channel = 0; channel < channels; ++channel)
                    for (std::uint32_t frame = 0; frame < frames; ++frame)
                        samples[channel * frames + frame] += buffer[channel * frames + frame];
            }
        }

    private:
        std::vector<float> buffer;
    };

    class Mixer final
    {
    public:
        class Event final
        {
        public:
            enum class Type
            {
                streamStarted,
                streamReset,
                streamStopped,
                starvation
            };

            Event() noexcept = default;
            explicit Event(Type initType) noexcept: type(initType) {}

            Type type;
            std::size_t objectId;
        };

        Mixer(std::uint32_t initBufferSize,
              std::uint32_t initChannels,
              const std::function<void(const Event&)>& initCallback);

        ~Mixer();

        Mixer(const Mixer&) = delete;
        Mixer& operator=(const Mixer&) = delete;

        Mixer(Mixer&&) = delete;
        Mixer& operator=(Mixer&&) = delete;

        void process();
        void getSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate, std::vector<float>& samples);

        using ObjectId = std::size_t;
        ObjectId getObjectId()
        {
            const auto i = deletedObjectIds.begin();

            if (i == deletedObjectIds.end())
                return ++lastObjectId; // zero is reserved for null node
            else
            {
                std::size_t objectId = *i;
                deletedObjectIds.erase(i);
                return objectId;
            }
        }

        void deleteObjectId(ObjectId objectId)
        {
            deletedObjectIds.insert(objectId);
        }

        void submitCommandBuffer(CommandBuffer&& commandBuffer)
        {
            std::unique_lock lock(commandQueueMutex);
            commandQueue.push(std::move(commandBuffer));
            lock.unlock();
        }

        auto getRootObjectId() const noexcept
        {
            return rootObjectId;
        }

    private:
        void mixerMain();

        std::uint32_t bufferSize;
        std::uint32_t channels;
        std::function<void(const Event&)> callback;

        ObjectId lastObjectId = 0;
        std::set<ObjectId> deletedObjectIds;

        std::vector<std::unique_ptr<Object>> objects;
        std::size_t rootObjectId = 0;
        RootObject* rootObject = nullptr;

        Bus* masterBus = nullptr;

        class Buffer final
        {
        public:
            Buffer(std::size_t size, std::uint32_t initChannels):
                maxFrames(size),
                channels(initChannels),
                buffer(size * channels)
            {
            }

            bool canRead() const { return frames > 0; }
            bool canWrite() const { return frames < maxFrames; }

        private:
            std::size_t frames = 0;
            std::size_t maxFrames;
            std::uint32_t channels;
            std::size_t readPosition = 0;
            std::size_t writePosition = 0;
            std::vector<float> buffer;
        };

        Thread mixerThread;
        std::mutex bufferMutex;
        std::condition_variable bufferCondition;
        Buffer buffer;

        std::queue<CommandBuffer> commandQueue;
        std::mutex commandQueueMutex;
    };
}

#endif // OUZEL_AUDIO_MIXER_MIXER_HPP
