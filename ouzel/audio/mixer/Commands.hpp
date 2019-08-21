// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_COMMANDS_HPP
#define OUZEL_AUDIO_MIXER_COMMANDS_HPP

#include <cstdint>
#include <functional>
#include <memory>

#include "audio/mixer/Processor.hpp"
#include "audio/mixer/Source.hpp"
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
                    DeleteObject,
                    AddChild,
                    RemoveChild,
                    Play,
                    Stop,
                    InitBus,
                    SetBusOutput,
                    SetMasterBus,
                    InitStream,
                    PlayStream,
                    StopStream,
                    SetStreamOutput,
                    InitData
                };

                explicit constexpr Command(Type initType) noexcept: type(initType) {}
                virtual ~Command() = default;

                const Type type;
            };

            class DeleteObjectCommand final: public Command
            {
            public:
                explicit constexpr DeleteObjectCommand(uintptr_t initObjectId) noexcept:
                    Command(Command::Type::DeleteObject),
                    objectId(initObjectId)
                {}

                const uintptr_t objectId;
            };

            class AddChildCommand final: public Command
            {
            public:
                constexpr AddChildCommand(uintptr_t initObjectId,
                                          uintptr_t initChildId) noexcept:
                    Command(Command::Type::AddChild),
                    objectId(initObjectId),
                    childId(initChildId)
                {}

                const uintptr_t objectId;
                const uintptr_t childId;
            };

            class RemoveChildCommand final: public Command
            {
            public:
                constexpr RemoveChildCommand(uintptr_t initObjectId,
                                             uintptr_t initChildId) noexcept:
                    Command(Command::Type::RemoveChild),
                    objectId(initObjectId),
                    childId(initChildId)
                {}

                const uintptr_t objectId;
                const uintptr_t childId;
            };

            class PlayCommand final: public Command
            {
            public:
                explicit constexpr PlayCommand(uintptr_t initObjectId) noexcept:
                    Command(Command::Type::Play),
                    objectId(initObjectId)
                {}

                const uintptr_t objectId;
            };

            class StopCommand final: public Command
            {
            public:
                constexpr StopCommand(uintptr_t initObjectId,
                                      bool initReset) noexcept:
                    Command(Command::Type::Stop),
                    objectId(initObjectId),
                    reset(initReset)
                {}

                const uintptr_t objectId;
                const bool reset;
            };

            class InitBusCommand final: public Command
            {
            public:
                explicit InitBusCommand(uintptr_t initBusId,
                                        std::unique_ptr<Processor> initProcessor) noexcept:
                    Command(Command::Type::InitBus),
                    busId(initBusId),
                    processor(std::move(initProcessor))
                {}

                const uintptr_t busId;
                std::unique_ptr<Processor> processor;
            };

            class SetBusOutputCommand final: public Command
            {
            public:
                constexpr SetBusOutputCommand(uintptr_t initBusId,
                                              uintptr_t initOutputBusId) noexcept:
                    Command(Command::Type::SetBusOutput),
                    busId(initBusId),
                    outputBusId(initOutputBusId)
                {}

                const uintptr_t busId;
                const uintptr_t outputBusId;
            };

            class SetMasterBusCommand final: public Command
            {
            public:
                explicit constexpr SetMasterBusCommand(uintptr_t initBusId) noexcept:
                    Command(Command::Type::SetMasterBus),
                    busId(initBusId)
                {}

                const uintptr_t busId;
            };

            class InitStreamCommand final: public Command
            {
            public:
                InitStreamCommand(uintptr_t initStreamId,
                                  std::unique_ptr<Emitter> initEmitter) noexcept:
                    Command(Command::Type::InitStream),
                    streamId(initStreamId),
                    emitter(std::move(initEmitter))
                {}

                const uintptr_t streamId;
                std::unique_ptr<Emitter> emitter;
            };

            class PlayStreamCommand final: public Command
            {
            public:
                explicit constexpr PlayStreamCommand(uintptr_t initStreamId) noexcept:
                    Command(Command::Type::PlayStream),
                    streamId(initStreamId)
                {}

                const uintptr_t streamId;
            };

            class StopStreamCommand final: public Command
            {
            public:
                constexpr StopStreamCommand(uintptr_t initStreamId,
                                            bool initReset) noexcept:
                    Command(Command::Type::StopStream),
                    streamId(initStreamId),
                    reset(initReset)
                {}

                const uintptr_t streamId;
                const bool reset;
            };

            class SetStreamOutputCommand final: public Command
            {
            public:
                constexpr SetStreamOutputCommand(uintptr_t initStreamId,
                                                 uintptr_t initBusId) noexcept:
                    Command(Command::Type::SetStreamOutput),
                    streamId(initStreamId),
                    busId(initBusId)
                {}

                const uintptr_t streamId;
                const uintptr_t busId;
            };

            class InitDataCommand final: public Command
            {
            public:
                InitDataCommand(uintptr_t initDataId,
                                std::unique_ptr<Data> initData) noexcept:
                    Command(Command::Type::InitData),
                    dataId(initDataId),
                    data(std::move(initData))
                {}

                const uintptr_t dataId;
                std::unique_ptr<Data> data;
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
