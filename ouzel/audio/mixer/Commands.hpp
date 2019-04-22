// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_COMMANDS_HPP
#define OUZEL_AUDIO_MIXER_COMMANDS_HPP

#include <cstdint>
#include <functional>
#include <memory>

#include "audio/mixer/Processor.hpp"
#include "audio/mixer/Stream.hpp"
#include "audio/mixer/Data.hpp"

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
                    INIT_DATA,
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
                                  uintptr_t initDataId):
                    Command(Command::Type::INIT_STREAM),
                    streamId(initStreamId),
                    dataId(initDataId)
                {}

                uintptr_t streamId;
                uintptr_t dataId;
            };

            class PlayStreamCommand final: public Command
            {
            public:
                PlayStreamCommand(uintptr_t initStreamId):
                    Command(Command::Type::PLAY_STREAM),
                    streamId(initStreamId)
                {}

                uintptr_t streamId;
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

            class InitDataCommand final: public Command
            {
            public:
                InitDataCommand(uintptr_t initDataId,
                                const std::function<std::unique_ptr<Data>()>& initInitFunction):
                    Command(Command::Type::INIT_DATA),
                    dataId(initDataId),
                    initFunction(initInitFunction)
                {}

                uintptr_t dataId;
                std::function<std::unique_ptr<Data>()> initFunction;
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

            class CommandBuffer final
            {
            public:
                CommandBuffer() {}
                CommandBuffer(const std::string& initName):
                    name(initName)
                {
                }

                inline const std::string& getName() const { return name; }

                inline bool isEmpty() const { return commands.empty(); }

                inline void pushCommand(std::unique_ptr<Command>&& command)
                {
                    commands.push(std::move(command));
                }

                inline std::unique_ptr<Command> popCommand()
                {
                    std::unique_ptr<Command> result = std::move(commands.front());
                    commands.pop();
                    return result;
                }

                inline const std::queue<std::unique_ptr<Command>>& getCommands() const
                {
                    return commands;
                }

            private:
                std::string name;
                std::queue<std::unique_ptr<Command>> commands;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_COMMANDS_HPP
