// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_COMMANDS_HPP
#define OUZEL_AUDIO_MIXER_COMMANDS_HPP

#include <cstdint>
#include <functional>
#include <memory>

namespace ouzel
{
    namespace audio
    {
        class Processor;
        class Source;
        class SourceData;

        class Command
        {
        public:
            enum class Type
            {
                DELETE_OBJECT,
                INIT_BUS,
                SET_BUS_OUTPUT,
                SET_MASTER_BUS,
                INIT_SOURCE,
                SET_SOURCE_OUTPUT,
                INIT_SOURCE_DATA,
                INIT_PROCESSOR,
                SET_PROCESSOR_BUS,
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
            InitSourceCommand(uintptr_t initSourceId):
                Command(Command::Type::INIT_SOURCE),
                sourceId(initSourceId)
            {}

            uintptr_t sourceId;
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
            InitSourceDataCommand(uintptr_t initSourceDataId):
                Command(Command::Type::INIT_SOURCE_DATA),
                sourceDataId(initSourceDataId)
            {}

            uintptr_t sourceDataId;
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

        class SetProcessorBusCommand: public Command
        {
        public:
            SetProcessorBusCommand(uintptr_t initProcessorId,
                                   uintptr_t initBusId):
                Command(Command::Type::SET_PROCESSOR_BUS),
                processorId(initProcessorId),
                busId(initBusId)
            {}

            uintptr_t processorId;
            uintptr_t busId;
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
