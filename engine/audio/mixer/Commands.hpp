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

namespace ouzel::audio::mixer
{
    class Command
    {
    public:
        enum class Type
        {
            initObject,
            deleteObject,
            addChild,
            removeChild,
            play,
            stop,
            initBus,
            setBusOutput,
            addProcessor,
            removeProcessor,
            setMasterBus,
            initStream,
            playStream,
            stopStream,
            setStreamOutput,
            initData,
            initProcessor,
            updateProcessor
        };

        explicit constexpr Command(Type initType) noexcept: type(initType) {}
        virtual ~Command() = default;

        const Type type;
    };

    class InitObjectCommand final: public Command
    {
    public:
        explicit constexpr InitObjectCommand(std::size_t initObjectId) noexcept:
            Command(Command::Type::initObject),
            objectId(initObjectId)
        {}

        InitObjectCommand(std::size_t initObjectId,
                          std::unique_ptr<Source> initSource) noexcept:
            Command(Command::Type::initObject),
            objectId(initObjectId),
            source(std::move(initSource))
        {}

        const std::size_t objectId;
        std::unique_ptr<Source> source;
    };

    class DeleteObjectCommand final: public Command
    {
    public:
        explicit constexpr DeleteObjectCommand(std::size_t initObjectId) noexcept:
            Command(Command::Type::deleteObject),
            objectId(initObjectId)
        {}

        const std::size_t objectId;
    };

    class AddChildCommand final: public Command
    {
    public:
        constexpr AddChildCommand(std::size_t initObjectId,
                                  std::size_t initChildId) noexcept:
            Command(Command::Type::addChild),
            objectId(initObjectId),
            childId(initChildId)
        {}

        const std::size_t objectId;
        const std::size_t childId;
    };

    class RemoveChildCommand final: public Command
    {
    public:
        constexpr RemoveChildCommand(std::size_t initObjectId,
                                     std::size_t initChildId) noexcept:
            Command(Command::Type::removeChild),
            objectId(initObjectId),
            childId(initChildId)
        {}

        const std::size_t objectId;
        const std::size_t childId;
    };

    class PlayCommand final: public Command
    {
    public:
        explicit constexpr PlayCommand(std::size_t initObjectId) noexcept:
            Command(Command::Type::play),
            objectId(initObjectId)
        {}

        const std::size_t objectId;
    };

    class StopCommand final: public Command
    {
    public:
        constexpr StopCommand(std::size_t initObjectId,
                              bool initReset) noexcept:
            Command(Command::Type::stop),
            objectId(initObjectId),
            reset(initReset)
        {}

        const std::size_t objectId;
        const bool reset;
    };

    class InitBusCommand final: public Command
    {
    public:
        explicit constexpr InitBusCommand(std::size_t initBusId) noexcept:
            Command(Command::Type::initBus),
            busId(initBusId)
        {}

        const std::size_t busId;
    };

    class SetBusOutputCommand final: public Command
    {
    public:
        constexpr SetBusOutputCommand(std::size_t initBusId,
                                      std::size_t initOutputBusId) noexcept:
            Command(Command::Type::setBusOutput),
            busId(initBusId),
            outputBusId(initOutputBusId)
        {}

        const std::size_t busId;
        const std::size_t outputBusId;
    };

    class AddProcessorCommand final: public Command
    {
    public:
        constexpr AddProcessorCommand(std::size_t initBusId,
                                      std::size_t initProcessorId) noexcept:
            Command(Command::Type::addProcessor),
            busId(initBusId),
            processorId(initProcessorId)
        {}

        const std::size_t busId;
        const std::size_t processorId;
    };

    class RemoveProcessorCommand final: public Command
    {
    public:
        constexpr RemoveProcessorCommand(std::size_t initBusId,
                                         std::size_t initProcessorId) noexcept:
            Command(Command::Type::removeProcessor),
            busId(initBusId),
            processorId(initProcessorId)
        {}

        const std::size_t busId;
        const std::size_t processorId;
    };

    class SetMasterBusCommand final: public Command
    {
    public:
        explicit constexpr SetMasterBusCommand(std::size_t initBusId) noexcept:
            Command(Command::Type::setMasterBus),
            busId(initBusId)
        {}

        const std::size_t busId;
    };

    class InitStreamCommand final: public Command
    {
    public:
        constexpr InitStreamCommand(std::size_t initStreamId,
                                    std::size_t initDataId) noexcept:
            Command(Command::Type::initStream),
            streamId(initStreamId),
            dataId(initDataId)
        {}

        const std::size_t streamId;
        const std::size_t dataId;
    };

    class PlayStreamCommand final: public Command
    {
    public:
        explicit constexpr PlayStreamCommand(std::size_t initStreamId) noexcept:
            Command(Command::Type::playStream),
            streamId(initStreamId)
        {}

        const std::size_t streamId;
    };

    class StopStreamCommand final: public Command
    {
    public:
        constexpr StopStreamCommand(std::size_t initStreamId,
                                    bool initReset) noexcept:
            Command(Command::Type::stopStream),
            streamId(initStreamId),
            reset(initReset)
        {}

        const std::size_t streamId;
        const bool reset;
    };

    class SetStreamOutputCommand final: public Command
    {
    public:
        constexpr SetStreamOutputCommand(std::size_t initStreamId,
                                         std::size_t initBusId) noexcept:
            Command(Command::Type::setStreamOutput),
            streamId(initStreamId),
            busId(initBusId)
        {}

        const std::size_t streamId;
        const std::size_t busId;
    };

    class InitDataCommand final: public Command
    {
    public:
        InitDataCommand(std::size_t initDataId,
                        std::unique_ptr<Data> initData) noexcept:
            Command(Command::Type::initData),
            dataId(initDataId),
            data(std::move(initData))
        {}

        const std::size_t dataId;
        std::unique_ptr<Data> data;
    };

    class InitProcessorCommand final: public Command
    {
    public:
        InitProcessorCommand(std::size_t initProcessorId,
                            std::unique_ptr<Processor> initProcessor) noexcept:
            Command(Command::Type::initProcessor),
            processorId(initProcessorId),
            processor(std::move(initProcessor))
        {}

        const std::size_t processorId;
        std::unique_ptr<Processor> processor;
    };

    class UpdateProcessorCommand final: public Command
    {
    public:
        UpdateProcessorCommand(std::size_t initProcessorId,
                               const std::function<void(Processor*)>& initUpdateFunction) noexcept:
            Command(Command::Type::updateProcessor),
            processorId(initProcessorId),
            updateFunction(initUpdateFunction)
        {}

        const std::size_t processorId;
        const std::function<void(Processor*)> updateFunction;
    };

    class CommandBuffer final
    {
    public:
        CommandBuffer() = default;
        explicit CommandBuffer(const std::string& initName) noexcept(false):
            name(initName)
        {
        }

        auto& getName() const noexcept { return name; }

        auto isEmpty() const noexcept { return commands.empty(); }

        void pushCommand(std::unique_ptr<Command> command)
        {
            commands.push(std::move(command));
        }

        std::unique_ptr<Command> popCommand()
        {
            auto result = std::move(commands.front());
            commands.pop();
            return result;
        }

        auto& getCommands() const
        {
            return commands;
        }

    private:
        std::string name;
        std::queue<std::unique_ptr<Command>> commands;
    };
}

#endif // OUZEL_AUDIO_MIXER_COMMANDS_HPP
