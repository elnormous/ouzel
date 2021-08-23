// Ouzel by Elviss Strazdins

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
    using ObjectId = std::size_t;

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

        explicit constexpr Command(Type initType) noexcept: type{initType} {}
        virtual ~Command() = default;

        const Type type;
    };

    class InitObjectCommand final: public Command
    {
    public:
        explicit constexpr InitObjectCommand(ObjectId initObjectId) noexcept:
            Command(Command::Type::initObject),
            objectId(initObjectId)
        {}

        InitObjectCommand(ObjectId initObjectId,
                          std::unique_ptr<Source> initSource) noexcept:
            Command(Command::Type::initObject),
            objectId(initObjectId),
            source(std::move(initSource))
        {}

        const ObjectId objectId;
        std::unique_ptr<Source> source;
    };

    class DeleteObjectCommand final: public Command
    {
    public:
        explicit constexpr DeleteObjectCommand(ObjectId initObjectId) noexcept:
            Command(Command::Type::deleteObject),
            objectId(initObjectId)
        {}

        const ObjectId objectId;
    };

    class AddChildCommand final: public Command
    {
    public:
        constexpr AddChildCommand(ObjectId initObjectId,
                                  ObjectId initChildId) noexcept:
            Command(Command::Type::addChild),
            objectId(initObjectId),
            childId(initChildId)
        {}

        const ObjectId objectId;
        const ObjectId childId;
    };

    class RemoveChildCommand final: public Command
    {
    public:
        constexpr RemoveChildCommand(ObjectId initObjectId,
                                     ObjectId initChildId) noexcept:
            Command(Command::Type::removeChild),
            objectId(initObjectId),
            childId(initChildId)
        {}

        const ObjectId objectId;
        const ObjectId childId;
    };

    class PlayCommand final: public Command
    {
    public:
        explicit constexpr PlayCommand(ObjectId initObjectId) noexcept:
            Command(Command::Type::play),
            objectId(initObjectId)
        {}

        const ObjectId objectId;
    };

    class StopCommand final: public Command
    {
    public:
        constexpr StopCommand(ObjectId initObjectId,
                              bool initReset) noexcept:
            Command(Command::Type::stop),
            objectId(initObjectId),
            reset(initReset)
        {}

        const ObjectId objectId;
        const bool reset;
    };

    class InitBusCommand final: public Command
    {
    public:
        explicit constexpr InitBusCommand(ObjectId initBusId) noexcept:
            Command(Command::Type::initBus),
            busId(initBusId)
        {}

        const ObjectId busId;
    };

    class SetBusOutputCommand final: public Command
    {
    public:
        constexpr SetBusOutputCommand(ObjectId initBusId,
                                      ObjectId initOutputBusId) noexcept:
            Command(Command::Type::setBusOutput),
            busId(initBusId),
            outputBusId(initOutputBusId)
        {}

        const ObjectId busId;
        const ObjectId outputBusId;
    };

    class AddProcessorCommand final: public Command
    {
    public:
        constexpr AddProcessorCommand(ObjectId initBusId,
                                      ObjectId initProcessorId) noexcept:
            Command(Command::Type::addProcessor),
            busId(initBusId),
            processorId(initProcessorId)
        {}

        const ObjectId busId;
        const ObjectId processorId;
    };

    class RemoveProcessorCommand final: public Command
    {
    public:
        constexpr RemoveProcessorCommand(ObjectId initBusId,
                                         ObjectId initProcessorId) noexcept:
            Command(Command::Type::removeProcessor),
            busId(initBusId),
            processorId(initProcessorId)
        {}

        const ObjectId busId;
        const ObjectId processorId;
    };

    class SetMasterBusCommand final: public Command
    {
    public:
        explicit constexpr SetMasterBusCommand(ObjectId initBusId) noexcept:
            Command(Command::Type::setMasterBus),
            busId(initBusId)
        {}

        const ObjectId busId;
    };

    class InitStreamCommand final: public Command
    {
    public:
        constexpr InitStreamCommand(ObjectId initStreamId,
                                    ObjectId initDataId) noexcept:
            Command(Command::Type::initStream),
            streamId(initStreamId),
            dataId(initDataId)
        {}

        const ObjectId streamId;
        const ObjectId dataId;
    };

    class PlayStreamCommand final: public Command
    {
    public:
        explicit constexpr PlayStreamCommand(ObjectId initStreamId) noexcept:
            Command(Command::Type::playStream),
            streamId(initStreamId)
        {}

        const ObjectId streamId;
    };

    class StopStreamCommand final: public Command
    {
    public:
        constexpr StopStreamCommand(ObjectId initStreamId,
                                    bool initReset) noexcept:
            Command(Command::Type::stopStream),
            streamId(initStreamId),
            reset(initReset)
        {}

        const ObjectId streamId;
        const bool reset;
    };

    class SetStreamOutputCommand final: public Command
    {
    public:
        constexpr SetStreamOutputCommand(ObjectId initStreamId,
                                         ObjectId initBusId) noexcept:
            Command(Command::Type::setStreamOutput),
            streamId(initStreamId),
            busId(initBusId)
        {}

        const ObjectId streamId;
        const ObjectId busId;
    };

    class InitDataCommand final: public Command
    {
    public:
        InitDataCommand(ObjectId initDataId,
                        std::unique_ptr<Data> initData) noexcept:
            Command(Command::Type::initData),
            dataId(initDataId),
            data(std::move(initData))
        {}

        const ObjectId dataId;
        std::unique_ptr<Data> data;
    };

    class InitProcessorCommand final: public Command
    {
    public:
        InitProcessorCommand(ObjectId initProcessorId,
                            std::unique_ptr<Processor> initProcessor) noexcept:
            Command(Command::Type::initProcessor),
            processorId(initProcessorId),
            processor(std::move(initProcessor))
        {}

        const ObjectId processorId;
        std::unique_ptr<Processor> processor;
    };

    class UpdateProcessorCommand final: public Command
    {
    public:
        UpdateProcessorCommand(ObjectId initProcessorId,
                               const std::function<void(Processor*)>& initUpdateFunction) noexcept:
            Command(Command::Type::updateProcessor),
            processorId(initProcessorId),
            updateFunction(initUpdateFunction)
        {}

        const ObjectId processorId;
        const std::function<void(Processor*)> updateFunction;
    };

    class CommandBuffer final
    {
    public:
        CommandBuffer() = default;
        explicit CommandBuffer(const std::string& initName) noexcept(false):
            name{initName}
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
