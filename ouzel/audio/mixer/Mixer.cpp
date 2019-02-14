// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Mixer.hpp"
#include "Bus.hpp"
#include "Stream.hpp"
#include "Source.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            Mixer::Mixer(const std::function<void(const Event&)>& initCallback):
                callback(initCallback)
            {
            }

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
                            auto deleteObjectCommand = static_cast<const DeleteObjectCommand*>(command.get());
                            objects[deleteObjectCommand->objectId - 1].reset();
                            break;
                        }
                        case Command::Type::INIT_BUS:
                        {
                            auto initBusCommand = static_cast<const InitBusCommand*>(command.get());

                            if (initBusCommand->busId > objects.size())
                                objects.resize(initBusCommand->busId);

                            objects[initBusCommand->busId - 1].reset(new Bus());
                            break;
                        }
                        case Command::Type::SET_BUS_OUTPUT:
                        {
                            auto setBusOutputCommand = static_cast<const SetBusOutputCommand*>(command.get());

                            Bus* bus = static_cast<Bus*>(objects[setBusOutputCommand->busId - 1].get());
                            bus->setOutput(setBusOutputCommand->outputBusId ? static_cast<Bus*>(objects[setBusOutputCommand->outputBusId - 1].get()) : nullptr);
                            break;
                        }
                        case Command::Type::ADD_PROCESSOR:
                        {
                            auto addProcessorCommand = static_cast<const AddProcessorCommand*>(command.get());

                            Bus* bus = static_cast<Bus*>(objects[addProcessorCommand->busId - 1].get());
                            Processor* processor = static_cast<Processor*>(objects[addProcessorCommand->processorId - 1].get());
                            bus->addProcessor(processor);
                            break;
                        }
                        case Command::Type::REMOVE_PROCESSOR:
                        {
                            auto removeProcessorCommand = static_cast<const RemoveProcessorCommand*>(command.get());

                            Bus* bus = static_cast<Bus*>(objects[removeProcessorCommand->busId - 1].get());
                            Processor* processor = static_cast<Processor*>(objects[removeProcessorCommand->processorId - 1].get());
                            bus->removeProcessor(processor);
                            break;
                        }
                        case Command::Type::SET_MASTER_BUS:
                        {
                            auto setMasterBusCommand = static_cast<const SetMasterBusCommand*>(command.get());

                            masterBus = setMasterBusCommand->busId ? static_cast<Bus*>(objects[setMasterBusCommand->busId - 1].get()) : nullptr;
                            break;
                        }
                        case Command::Type::INIT_STREAM:
                        {
                            auto initStreamCommand = static_cast<const InitStreamCommand*>(command.get());

                            if (initStreamCommand->streamId > objects.size())
                                objects.resize(initStreamCommand->streamId);

                            Source* source = static_cast<Source*>(objects[initStreamCommand->sourceId - 1].get());
                            objects[initStreamCommand->streamId - 1] = source->createStream();
                            break;
                        }
                        case Command::Type::PLAY_STREAM:
                        {
                            auto playStreamCommand = static_cast<const PlayStreamCommand*>(command.get());

                            Stream* stream = static_cast<Stream*>(objects[playStreamCommand->streamId - 1].get());
                            stream->play(playStreamCommand->repeat);
                            break;
                        }
                        case Command::Type::STOP_STREAM:
                        {
                            auto stopStreamCommand = static_cast<const StopStreamCommand*>(command.get());

                            Stream* stream = static_cast<Stream*>(objects[stopStreamCommand->streamId - 1].get());
                            stream->stop(stopStreamCommand->reset);
                            break;
                        }
                        case Command::Type::SET_STREAM_OUTPUT:
                        {
                            auto setStreamOutputCommand = static_cast<const SetStreamOutputCommand*>(command.get());

                            Stream* stream = static_cast<Stream*>(objects[setStreamOutputCommand->streamId - 1].get());
                            stream->setOutput(setStreamOutputCommand->busId ? static_cast<Bus*>(objects[setStreamOutputCommand->busId - 1].get()) : nullptr);
                            break;
                        }
                        case Command::Type::INIT_SOURCE:
                        {
                            auto initSourceCommand = static_cast<const InitSourceCommand*>(command.get());

                            if (initSourceCommand->sourceId > objects.size())
                                objects.resize(initSourceCommand->sourceId);

                            objects[initSourceCommand->sourceId - 1] = initSourceCommand->initFunction();
                            break;
                        }
                        case Command::Type::INIT_PROCESSOR:
                        {
                            auto initProcessorCommand = static_cast<InitProcessorCommand*>(command.get());

                            if (initProcessorCommand->processorId > objects.size())
                                objects.resize(initProcessorCommand->processorId);

                            objects[initProcessorCommand->processorId - 1] = std::move(initProcessorCommand->processor);
                            break;
                        }
                        case Command::Type::UPDATE_PROCESSOR:
                        {
                            auto updateProcessorCommand = static_cast<const UpdateProcessorCommand*>(command.get());

                            Processor* processor = static_cast<Processor*>(objects[updateProcessorCommand->processorId - 1].get());
                            updateProcessorCommand->updateFunction(processor);
                            break;
                        }
                        default:
                            throw std::runtime_error("Invalid command");
                    }
                }
            }

            void Mixer::getData(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)
            {
                process();

                if (masterBus)
                {
                    Vector3F listenerPosition;
                    QuaternionF listenerRotation;

                    masterBus->getData(frames, channels, sampleRate, listenerPosition, listenerRotation, samples);
                }

                for (float& f : samples)
                    f = clamp(f, -1.0F, 1.0F);
            }
        }
    } // namespace audio
} // namespace ouzel
