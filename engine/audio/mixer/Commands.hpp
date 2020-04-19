// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_COMMANDS_HPP
#define OUZEL_AUDIO_MIXER_COMMANDS_HPP

#include <cstdint>
#include <functional>
#include <memory>

#include "Processor.hpp"
#include "Source.hpp"
#include "Stream.hpp"
#include "Data.hpp"

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
                    InitObject,
                    DeleteObject,
                    AddChild,
                    RemoveChild,
                    Play,
                    Stop,
                    InitBus,
                    SetBusOutput,
                    AddProcessor,
                    RemoveProcessor,
                    SetMasterBus,
                    InitStream,
                    PlayStream,
                    StopStream,
                    SetStreamOutput,
                    InitData,
                    InitProcessor,
                    UpdateProcessor
                };

                explicit constexpr Command(Type initType) noexcept: type(initType) {}
                virtual ~Command() = default;

                const Type type;
            };

            class InitObjectCommand final: public Command
            {
            public:
                explicit constexpr InitObjectCommand(std::uintptr_t initObjectId) noexcept:
                    Command(Command::Type::InitObject),
                    objectId(initObjectId)
                {}

                InitObjectCommand(std::uintptr_t initObjectId,
                                  std::unique_ptr<Source> initSource) noexcept:
                    Command(Command::Type::InitObject),
                    objectId(initObjectId),
                    source(std::move(initSource))
                {}

                const std::uintptr_t objectId;
                std::unique_ptr<Source> source;
            };

            class DeleteObjectCommand final: public Command
            {
            public:
                explicit constexpr DeleteObjectCommand(std::uintptr_t initObjectId) noexcept:
                    Command(Command::Type::DeleteObject),
                    objectId(initObjectId)
                {}

                const std::uintptr_t objectId;
            };

            class AddChildCommand final: public Command
            {
            public:
                constexpr AddChildCommand(std::uintptr_t initObjectId,
                                          std::uintptr_t initChildId) noexcept:
                    Command(Command::Type::AddChild),
                    objectId(initObjectId),
                    childId(initChildId)
                {}

                const std::uintptr_t objectId;
                const std::uintptr_t childId;
            };

            class RemoveChildCommand final: public Command
            {
            public:
                constexpr RemoveChildCommand(std::uintptr_t initObjectId,
                                             std::uintptr_t initChildId) noexcept:
                    Command(Command::Type::RemoveChild),
                    objectId(initObjectId),
                    childId(initChildId)
                {}

                const std::uintptr_t objectId;
                const std::uintptr_t childId;
            };

            class PlayCommand final: public Command
            {
            public:
                explicit constexpr PlayCommand(std::uintptr_t initObjectId) noexcept:
                    Command(Command::Type::Play),
                    objectId(initObjectId)
                {}

                const std::uintptr_t objectId;
            };

            class StopCommand final: public Command
            {
            public:
                constexpr StopCommand(std::uintptr_t initObjectId,
                                      bool initReset) noexcept:
                    Command(Command::Type::Stop),
                    objectId(initObjectId),
                    reset(initReset)
                {}

                const std::uintptr_t objectId;
                const bool reset;
            };

            class InitBusCommand final: public Command
            {
            public:
                explicit constexpr InitBusCommand(std::uintptr_t initBusId) noexcept:
                    Command(Command::Type::InitBus),
                    busId(initBusId)
                {}

                const std::uintptr_t busId;
            };

            class SetBusOutputCommand final: public Command
            {
            public:
                constexpr SetBusOutputCommand(std::uintptr_t initBusId,
                                              std::uintptr_t initOutputBusId) noexcept:
                    Command(Command::Type::SetBusOutput),
                    busId(initBusId),
                    outputBusId(initOutputBusId)
                {}

                const std::uintptr_t busId;
                const std::uintptr_t outputBusId;
            };

            class AddProcessorCommand final: public Command
            {
            public:
                constexpr AddProcessorCommand(std::uintptr_t initBusId,
                                              std::uintptr_t initProcessorId) noexcept:
                    Command(Command::Type::AddProcessor),
                    busId(initBusId),
                    processorId(initProcessorId)
                {}

                const std::uintptr_t busId;
                const std::uintptr_t processorId;
            };

            class RemoveProcessorCommand final: public Command
            {
            public:
                constexpr RemoveProcessorCommand(std::uintptr_t initBusId,
                                                 std::uintptr_t initProcessorId) noexcept:
                    Command(Command::Type::RemoveProcessor),
                    busId(initBusId),
                    processorId(initProcessorId)
                {}

                const std::uintptr_t busId;
                const std::uintptr_t processorId;
            };

            class SetMasterBusCommand final: public Command
            {
            public:
                explicit constexpr SetMasterBusCommand(std::uintptr_t initBusId) noexcept:
                    Command(Command::Type::SetMasterBus),
                    busId(initBusId)
                {}

                const std::uintptr_t busId;
            };

            class InitStreamCommand final: public Command
            {
            public:
                constexpr InitStreamCommand(std::uintptr_t initStreamId,
                                            std::uintptr_t initDataId) noexcept:
                    Command(Command::Type::InitStream),
                    streamId(initStreamId),
                    dataId(initDataId)
                {}

                const std::uintptr_t streamId;
                const std::uintptr_t dataId;
            };

            class PlayStreamCommand final: public Command
            {
            public:
                explicit constexpr PlayStreamCommand(std::uintptr_t initStreamId) noexcept:
                    Command(Command::Type::PlayStream),
                    streamId(initStreamId)
                {}

                const std::uintptr_t streamId;
            };

            class StopStreamCommand final: public Command
            {
            public:
                constexpr StopStreamCommand(std::uintptr_t initStreamId,
                                            bool initReset) noexcept:
                    Command(Command::Type::StopStream),
                    streamId(initStreamId),
                    reset(initReset)
                {}

                const std::uintptr_t streamId;
                const bool reset;
            };

            class SetStreamOutputCommand final: public Command
            {
            public:
                constexpr SetStreamOutputCommand(std::uintptr_t initStreamId,
                                                 std::uintptr_t initBusId) noexcept:
                    Command(Command::Type::SetStreamOutput),
                    streamId(initStreamId),
                    busId(initBusId)
                {}

                const std::uintptr_t streamId;
                const std::uintptr_t busId;
            };

            class InitDataCommand final: public Command
            {
            public:
                InitDataCommand(std::uintptr_t initDataId,
                                std::unique_ptr<Data> initData) noexcept:
                    Command(Command::Type::InitData),
                    dataId(initDataId),
                    data(std::move(initData))
                {}

                const std::uintptr_t dataId;
                std::unique_ptr<Data> data;
            };

            class InitProcessorCommand final: public Command
            {
            public:
                InitProcessorCommand(std::uintptr_t initProcessorId,
                                    std::unique_ptr<Processor> initProcessor) noexcept:
                    Command(Command::Type::InitProcessor),
                    processorId(initProcessorId),
                    processor(std::move(initProcessor))
                {}

                const std::uintptr_t processorId;
                std::unique_ptr<Processor> processor;
            };

            class UpdateProcessorCommand final: public Command
            {
            public:
                UpdateProcessorCommand(std::uintptr_t initProcessorId,
                                       const std::function<void(Processor*)>& initUpdateFunction) noexcept:
                    Command(Command::Type::UpdateProcessor),
                    processorId(initProcessorId),
                    updateFunction(initUpdateFunction)
                {}

                const std::uintptr_t processorId;
                const std::function<void(Processor*)> updateFunction;
            };

            class CommandBuffer final
            {
            public:
                CommandBuffer() = default;
                explicit CommandBuffer(const std::string& initName) noexcept:
                    name(initName)
                {
                }

                inline auto& getName() const noexcept { return name; }

                inline auto isEmpty() const noexcept { return commands.empty(); }

                inline void pushCommand(std::unique_ptr<Command> command)
                {
                    commands.push(std::move(command));
                }

                inline std::unique_ptr<Command> popCommand()
                {
                    auto result = std::move(commands.front());
                    commands.pop();
                    return result;
                }

                inline auto& getCommands() const
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
