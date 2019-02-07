// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_COMMANDS_HPP
#define OUZEL_AUDIO_MIXER_COMMANDS_HPP

#include <cstdint>
#include <functional>
#include <memory>

#include "audio/mixer/Processor.hpp"
#include "audio/mixer/Stream.hpp"
#include "audio/mixer/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
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
                    INIT_STREAM,
                    PLAY_STREAM,
                    STOP_STREAM,
                    SET_STREAM_OUTPUT,
                    INIT_SOURCE,
                    INIT_PROCESSOR,
                    UPDATE_PROCESSOR
                };

                explicit Command(Type initType): type(initType) {}

                Type type;
            };

            class DeleteObjectCommand final: public Command
            {
            public:
                DeleteObjectCommand(uintptr_t initObjectId):
                    Command(Command::Type::DELETE_OBJECT),
                    objectId(initObjectId)
                {}

                uintptr_t objectId;
            };

            class InitBusCommand final: public Command
            {
            public:
                InitBusCommand(uintptr_t initBusId):
                    Command(Command::Type::INIT_BUS),
                    busId(initBusId)
                {}

                uintptr_t busId;
            };

            class SetBusOutputCommand final: public Command
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

            class AddProcessorCommand final: public Command
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

            class RemoveProcessorCommand final: public Command
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

            class SetMasterBusCommand final: public Command
            {
            public:
                SetMasterBusCommand(uintptr_t initBusId):
                    Command(Command::Type::SET_MASTER_BUS),
                    busId(initBusId)
                {}

                uintptr_t busId;
            };

            class InitStreamCommand final: public Command
            {
            public:
                InitStreamCommand(uintptr_t initStreamId,
                                  uintptr_t initSourceId):
                    Command(Command::Type::INIT_STREAM),
                    streamId(initStreamId),
                    sourceId(initSourceId)
                {}

                uintptr_t streamId;
                uintptr_t sourceId;
            };

            class PlayStreamCommand final: public Command
            {
            public:
                PlayStreamCommand(uintptr_t initStreamId,
                                  bool initRepeat):
                    Command(Command::Type::PLAY_STREAM),
                    streamId(initStreamId),
                    repeat(initRepeat)
                {}

                uintptr_t streamId;
                bool repeat;
            };

            class StopStreamCommand final: public Command
            {
            public:
                StopStreamCommand(uintptr_t initStreamId,
                                  bool initReset):
                    Command(Command::Type::STOP_STREAM),
                    streamId(initStreamId),
                    reset(initReset)
                {}

                uintptr_t streamId;
                bool reset;
            };

            class SetStreamOutputCommand final: public Command
            {
            public:
                SetStreamOutputCommand(uintptr_t initStreamId,
                                       uintptr_t initBusId):
                    Command(Command::Type::SET_STREAM_OUTPUT),
                    streamId(initStreamId),
                    busId(initBusId)
                {}

                uintptr_t streamId;
                uintptr_t busId;
            };

            class InitSourceCommand final: public Command
            {
            public:
                InitSourceCommand(uintptr_t initSourceId,
                                  const std::function<std::unique_ptr<Source>()>& initInitFunction):
                    Command(Command::Type::INIT_SOURCE),
                    sourceId(initSourceId),
                    initFunction(initInitFunction)
                {}

                uintptr_t sourceId;
                std::function<std::unique_ptr<Source>()> initFunction;
            };

            class InitProcessorCommand final: public Command
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

            class UpdateProcessorCommand final: public Command
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
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_COMMANDS_HPP
