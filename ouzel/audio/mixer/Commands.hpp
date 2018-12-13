// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_COMMANDS_HPP
#define OUZEL_AUDIO_MIXER_COMMANDS_HPP

#include <cstdint>
#include <functional>
#include <memory>

#include "audio/mixer/Processor.hpp"
#include "audio/mixer/Source.hpp"
#include "audio/mixer/SourceData.hpp"

namespace ouzel
{
    namespace audio
    {
        class Command
        {
        public:
            enum class Type
            {
                DELETE_OBJECT,
                INIT_BUS,
                SET_BUS_OUTPUT,
                ADD_PROCESSOR,
                REMOVE_PROCESSOR,
                SET_MASTER_BUS,
                INIT_SOURCE,
                PLAY_SOURCE,
                STOP_SOURCE,
                SET_SOURCE_OUTPUT,
                INIT_SOURCE_DATA,
                INIT_PROCESSOR,
                UPDATE_PROCESSOR
            };

            explicit Command(Type initType): type(initType) {}

            Type type;
        };

        class DeleteObjectCommand: public Command
        {
        public:
            DeleteObjectCommand(uintptr_t initObjectId):
                Command(Command::Type::DELETE_OBJECT),
                objectId(initObjectId)
            {}

            uintptr_t objectId;
        };

        class InitBusCommand: public Command
        {
        public:
            InitBusCommand(uintptr_t initBusId):
                Command(Command::Type::INIT_BUS),
                busId(initBusId)
            {}

            uintptr_t busId;
        };

        class SetBusOutputCommand: public Command
        {
        public:
            SetBusOutputCommand(uintptr_t initBusId,
                                uintptr_t initOutputBusId):
                Command(Command::Type::SET_BUS_OUTPUT),
                busId(initBusId),
                outputBusId(initOutputBusId)
            {}

            uintptr_t busId;
            uintptr_t outputBusId;
        };

        class AddProcessorCommand: public Command
        {
        public:
            AddProcessorCommand(uintptr_t initBusId,
                                uintptr_t initProcessorId):
                Command(Command::Type::ADD_PROCESSOR),
                busId(initBusId),
                processorId(initProcessorId)
            {}

            uintptr_t busId;
            uintptr_t processorId;
        };

        class RemoveProcessorCommand: public Command
        {
        public:
            RemoveProcessorCommand(uintptr_t initBusId,
                                   uintptr_t initProcessorId):
                Command(Command::Type::REMOVE_PROCESSOR),
                busId(initBusId),
                processorId(initProcessorId)
            {}

            uintptr_t busId;
            uintptr_t processorId;
        };

        class SetMasterBusCommand: public Command
        {
        public:
            SetMasterBusCommand(uintptr_t initBusId):
                Command(Command::Type::SET_MASTER_BUS),
                busId(initBusId)
            {}

            uintptr_t busId;
        };

        class InitSourceCommand: public Command
        {
        public:
            InitSourceCommand(uintptr_t initSourceId,
                              uintptr_t initSourceDataId):
                Command(Command::Type::INIT_SOURCE),
                sourceId(initSourceId),
                sourceDataId(initSourceDataId)
            {}

            uintptr_t sourceId;
            uintptr_t sourceDataId;
        };

        class PlaySourceCommand: public Command
        {
        public:
            PlaySourceCommand(uintptr_t initSourceId,
                              bool initRepeat):
                Command(Command::Type::PLAY_SOURCE),
                sourceId(initSourceId),
                repeat(initRepeat)
            {}

            uintptr_t sourceId;
            bool repeat;
        };

        class StopSourceCommand: public Command
        {
        public:
            StopSourceCommand(uintptr_t initSourceId,
                              bool initReset):
                Command(Command::Type::STOP_SOURCE),
                sourceId(initSourceId),
                reset(initReset)
            {}

            uintptr_t sourceId;
            bool reset;
        };

        class SetSourceOutputCommand: public Command
        {
        public:
            SetSourceOutputCommand(uintptr_t initSourceId,
                                   uintptr_t initBusId):
                Command(Command::Type::SET_SOURCE_OUTPUT),
                sourceId(initSourceId),
                busId(initBusId)
            {}

            uintptr_t sourceId;
            uintptr_t busId;
        };

        class InitSourceDataCommand: public Command
        {
        public:
            InitSourceDataCommand(uintptr_t initSourceDataId,
                                  std::unique_ptr<SourceData>&& initSourceData):
                Command(Command::Type::INIT_SOURCE_DATA),
                sourceDataId(initSourceDataId),
                sourceData(std::forward<std::unique_ptr<SourceData>>(initSourceData))
            {}

            uintptr_t sourceDataId;
            std::unique_ptr<SourceData> sourceData;
        };

        class InitProcessorCommand: public Command
        {
        public:
            InitProcessorCommand(uintptr_t initProcessorId,
                                std::unique_ptr<Processor>&& initProcessor):
                Command(Command::Type::INIT_PROCESSOR),
                processorId(initProcessorId),
                processor(std::forward<std::unique_ptr<Processor>>(initProcessor))
            {}

            uintptr_t processorId;
            std::unique_ptr<Processor> processor;
        };

        class UpdateProcessorCommand: public Command
        {
        public:
            UpdateProcessorCommand(uintptr_t initProcessorId,
                                   std::function<void(Processor*)> initUpdateFunction):
                Command(Command::Type::UPDATE_PROCESSOR),
                processorId(initProcessorId),
                updateFunction(initUpdateFunction)
            {}

            uintptr_t processorId;
            std::function<void(Processor*)> updateFunction;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_COMMANDS_HPP
