// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Mixer.hpp"
#include "Bus.hpp"
#include "Source.hpp"
#include "SourceData.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        void Mixer::addCommand(std::unique_ptr<Command>&& command)
        {
            std::unique_lock<std::mutex> lock(commandMutex);
            commandQueue.push(std::move(command));
            lock.unlock();
            commandConditionVariable.notify_all();
        }

        void Mixer::process()
        {
            std::unique_ptr<Command> command;
            for (;;)
            {
                std::unique_lock<std::mutex> lock(commandMutex);
                if (commandQueue.empty()) break;
                command = std::move(commandQueue.front());
                commandQueue.pop();
                lock.unlock();

                switch (command->type)
                {
                    case Command::Type::DELETE_OBJECT:
                    {
                        DeleteObjectCommand* deleteObjectCommand = static_cast<DeleteObjectCommand*>(command.get());
                        objects[deleteObjectCommand->objectId - 1].reset();
                        break;
                    }
                    case Command::Type::INIT_BUS:
                    {
                        InitBusCommand* initBusCommand = static_cast<InitBusCommand*>(command.get());

                        if (initBusCommand->busId > objects.size())
                            objects.resize(initBusCommand->busId);

                        objects[initBusCommand->busId - 1].reset(new Bus());
                        break;
                    }
                    case Command::Type::SET_BUS_OUTPUT:
                    {
                        SetBusOutputCommand* setBusOutputCommand = static_cast<SetBusOutputCommand*>(command.get());

                        Bus* bus = static_cast<Bus*>(objects[setBusOutputCommand->busId - 1].get());
                        bus->setOutput(setBusOutputCommand->outputBusId ? static_cast<Bus*>(objects[setBusOutputCommand->outputBusId - 1].get()) : nullptr);
                        break;
                    }
                    case Command::Type::SET_MASTER_BUS:
                    {
                        SetMasterBusCommand* setMasterBusCommand = static_cast<SetMasterBusCommand*>(command.get());

                        masterBus = setMasterBusCommand->busId ? static_cast<Bus*>(objects[setMasterBusCommand->busId - 1].get()) : nullptr;
                        break;
                    }
                    case Command::Type::INIT_SOURCE:
                    {
                        InitSourceCommand* initSourceCommand = static_cast<InitSourceCommand*>(command.get());

                        if (initSourceCommand->sourceId > objects.size())
                            objects.resize(initSourceCommand->sourceId);

                        SourceData* sourceData = static_cast<SourceData*>(objects[initSourceCommand->sourceDataId - 1].get());
                        objects[initSourceCommand->sourceId - 1] = sourceData->createSource();
                        break;
                    }
                    case Command::Type::PLAY_SOURCE:
                    {
                        PlaySourceCommand* playSourceCommand = static_cast<PlaySourceCommand*>(command.get());

                        Source* source = static_cast<Source*>(objects[playSourceCommand->sourceId - 1].get());
                        source->play(playSourceCommand->repeat);
                        break;
                    }
                    case Command::Type::STOP_SOURCE:
                    {
                        StopSourceCommand* stopSourceCommand = static_cast<StopSourceCommand*>(command.get());

                        Source* source = static_cast<Source*>(objects[stopSourceCommand->sourceId - 1].get());
                        source->stop(stopSourceCommand->reset);
                        break;
                    }
                    case Command::Type::INIT_SOURCE_DATA:
                    {
                        InitSourceDataCommand* initSourceDataCommand = static_cast<InitSourceDataCommand*>(command.get());

                        if (initSourceDataCommand->sourceDataId > objects.size())
                            objects.resize(initSourceDataCommand->sourceDataId);

                        objects[initSourceDataCommand->sourceDataId - 1] = std::move(initSourceDataCommand->sourceData);
                        break;
                    }
                    case Command::Type::SET_SOURCE_OUTPUT:
                    {
                        SetSourceOutputCommand* setSourceOutputCommand = static_cast<SetSourceOutputCommand*>(command.get());
                        
                        Source* source = static_cast<Source*>(objects[setSourceOutputCommand->sourceId - 1].get());
                        source->setOutput(setSourceOutputCommand->busId ? static_cast<Bus*>(objects[setSourceOutputCommand->busId - 1].get()) : nullptr);
                        break;
                    }
                    case Command::Type::INIT_PROCESSOR:
                    {
                        InitProcessorCommand* initProcessorCommand = static_cast<InitProcessorCommand*>(command.get());

                        if (initProcessorCommand->processorId > objects.size())
                            objects.resize(initProcessorCommand->processorId);

                        objects[initProcessorCommand->processorId - 1] = std::move(initProcessorCommand->processor);
                        break;
                    }
                    case Command::Type::SET_PROCESSOR_BUS:
                    {
                        SetProcessorBusCommand* setProcessorBusCommand = static_cast<SetProcessorBusCommand*>(command.get());

                        Processor* processor = static_cast<Processor*>(objects[setProcessorBusCommand->processorId - 1].get());
                        processor->setBus(setProcessorBusCommand->busId ? static_cast<Bus*>(objects[setProcessorBusCommand->busId - 1].get()) : nullptr);
                        break;
                    }
                    case Command::Type::UPDATE_PROCESSOR:
                    {
                        UpdateProcessorCommand* updateProcessorCommand = static_cast<UpdateProcessorCommand*>(command.get());

                        Processor* processor = static_cast<Processor*>(objects[updateProcessorCommand->processorId - 1].get());
                        updateProcessorCommand->updateFunction(processor);
                        break;
                    }
                    default:
                        throw std::runtime_error("Invalid command");
                }
            }
        }

        void Mixer::getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& result)
        {
            if (masterBus)
            {
                Vector3 inputPosition;

                masterBus->getData(result, channels, sampleRate, inputPosition);
            }

            for (float& f : result)
                f = clamp(f, -1.0F, 1.0F);
        }
    } // namespace audio
} // namespace ouzel
