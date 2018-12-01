// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICE_HPP
#define OUZEL_AUDIO_AUDIODEVICE_HPP

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <vector>
#include "audio/Driver.hpp"
#include "audio/Object.hpp"
#include "audio/SampleFormat.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class ListenerObject;
        class MixerObject;

        class AudioDevice
        {
        public:
            struct Command
            {
                enum class Type
                {
                    DELETE_OBJECT,
                    INIT_BUS,
                    ADD_LISTENER,
                    REMOVE_LISTENER,
                    ADD_FILTER,
                    REMOVE_FILTER,
                    INIT_LISTENER,
                    UPDATE_LISTENER,
                    INIT_FILTER,
                    UPDATE_FILTER,
                    ADD_OUTPUT_BUS,
                    SET_MASTER_BUS
                };

                Command() {}
                explicit Command(Type initType): type(initType) {}

                Type type;
                uintptr_t objectId;
                uintptr_t destinationObjectId;
                std::function<std::unique_ptr<Object>(void)> createFunction;
                std::function<void(Object*)> updateFunction;
            };

            explicit AudioDevice(Driver initDriver);
            virtual ~AudioDevice();

            AudioDevice(const AudioDevice&) = delete;
            AudioDevice& operator=(const AudioDevice&) = delete;

            AudioDevice(AudioDevice&&) = delete;
            AudioDevice& operator=(AudioDevice&&) = delete;

            inline uint16_t getAPIMajorVersion() const { return apiMajorVersion; }
            inline uint16_t getAPIMinorVersion() const { return apiMinorVersion; }

            virtual void process();

            void addCommand(const Command& command);

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

        protected:
            void getData(uint32_t frames, std::vector<uint8_t>& result);

            uint16_t apiMajorVersion = 0;
            uint16_t apiMinorVersion = 0;
            
            SampleFormat sampleFormat = SampleFormat::SINT16;
            const uint32_t bufferSize = 2 * 4096;
            const uint32_t sampleRate = 44100;
            const uint16_t channels = 2;

        private:
            std::vector<std::vector<float>> buffers;
            uint32_t currentBuffer = 0;

            uintptr_t lastObjectId = 0;
            std::set<uintptr_t> deletedObjectIds;

            std::vector<std::unique_ptr<Object>> objects;

            Driver driver;

            Object* masterBus = nullptr;
            std::mutex commandMutex;
            std::condition_variable commandConditionVariable;
            std::queue<Command> commandQueue;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_AUDIODEVICE_HPP
