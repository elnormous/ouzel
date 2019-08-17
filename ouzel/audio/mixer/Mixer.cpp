// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Mixer.hpp"
#include "Bus.hpp"
#include "Data.hpp"
#include "Stream.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            Mixer::Mixer(uint32_t initBufferSize,
                         uint16_t initChannels,
                         const std::function<void(const Event&)>& initCallback):
                bufferSize(initBufferSize),
                channels(initChannels),
                callback(initCallback),
                mixerThread(&Mixer::mixerMain, this),
                buffer(initBufferSize * 3, initChannels)
            {
                //mixerThread.setPriority(20.0F, true);

                rootObjectId = getObjectId();
                objects.resize(rootObjectId);
                auto object = std::make_unique<RootObject>();
                rootObject = object.get();
                objects[rootObjectId - 1] = std::move(object);
            }

            Mixer::~Mixer()
            {
                if (mixerThread.isJoinable())
                    mixerThread.join();
            }

            void Mixer::process()
            {
                CommandBuffer commandBuffer;
                std::unique_ptr<Command> command;

                for (;;)
                {
                    std::unique_lock<std::mutex> lock(commandQueueMutex);
                    if (commandQueue.empty()) break;
                    commandBuffer = std::move(commandQueue.front());
                    commandQueue.pop();
                    lock.unlock();

                    while (!commandBuffer.isEmpty())
                    {
                        command = commandBuffer.popCommand();

                        switch (command->type)
                        {
                            case Command::Type::InitObject:
                            {
                                auto initObjectCommand = static_cast<InitObjectCommand*>(command.get());
                                objects[initObjectCommand->objectId - 1] = std::make_unique<Object>(std::move(initObjectCommand->source));
                                break;
                            }
                            case Command::Type::DeleteObject:
                            {
                                auto deleteObjectCommand = static_cast<const DeleteObjectCommand*>(command.get());
                                objects[deleteObjectCommand->objectId - 1].reset();
                                break;
                            }
                            case Command::Type::AddChild:
                            {
                                auto addChildCommand = static_cast<const AddChildCommand*>(command.get());
                                Object* object = objects[addChildCommand->objectId - 1].get();
                                Object* child = objects[addChildCommand->objectId - 1].get();
                                object->addChild(*child);
                                break;
                            }
                            case Command::Type::RemoveChild:
                            {
                                auto removeChildCommand = static_cast<const RemoveChildCommand*>(command.get());
                                Object* object = objects[removeChildCommand->objectId - 1].get();
                                Object* child = objects[removeChildCommand->objectId - 1].get();
                                object->removeChild(*child);
                                break;
                            }
                            case Command::Type::InitBus:
                            {
                                auto initBusCommand = static_cast<const InitBusCommand*>(command.get());

                                if (initBusCommand->busId > objects.size())
                                    objects.resize(initBusCommand->busId);

                                objects[initBusCommand->busId - 1] = std::make_unique<Bus>();
                                break;
                            }
                            case Command::Type::SetBusOutput:
                            {
                                auto setBusOutputCommand = static_cast<const SetBusOutputCommand*>(command.get());

                                Bus* bus = static_cast<Bus*>(objects[setBusOutputCommand->busId - 1].get());
                                bus->setOutput(setBusOutputCommand->outputBusId ? static_cast<Bus*>(objects[setBusOutputCommand->outputBusId - 1].get()) : nullptr);
                                break;
                            }
                            case Command::Type::AddProcessor:
                            {
                                auto addProcessorCommand = static_cast<const AddProcessorCommand*>(command.get());

                                Bus* bus = static_cast<Bus*>(objects[addProcessorCommand->busId - 1].get());
                                Processor* processor = static_cast<Processor*>(objects[addProcessorCommand->processorId - 1].get());
                                bus->addProcessor(processor);
                                break;
                            }
                            case Command::Type::RemoveProcessor:
                            {
                                auto removeProcessorCommand = static_cast<const RemoveProcessorCommand*>(command.get());

                                Bus* bus = static_cast<Bus*>(objects[removeProcessorCommand->busId - 1].get());
                                Processor* processor = static_cast<Processor*>(objects[removeProcessorCommand->processorId - 1].get());
                                bus->removeProcessor(processor);
                                break;
                            }
                            case Command::Type::SetMasterBus:
                            {
                                auto setMasterBusCommand = static_cast<const SetMasterBusCommand*>(command.get());

                                masterBus = setMasterBusCommand->busId ? static_cast<Bus*>(objects[setMasterBusCommand->busId - 1].get()) : nullptr;
                                break;
                            }
                            case Command::Type::InitStream:
                            {
                                auto initStreamCommand = static_cast<const InitStreamCommand*>(command.get());

                                if (initStreamCommand->streamId > objects.size())
                                    objects.resize(initStreamCommand->streamId);

                                Data* data = static_cast<Data*>(objects[initStreamCommand->dataId - 1].get());
                                objects[initStreamCommand->streamId - 1] = data->createStream();
                                break;
                            }
                            case Command::Type::PlayStream:
                            {
                                auto playStreamCommand = static_cast<const PlayStreamCommand*>(command.get());

                                Stream* stream = static_cast<Stream*>(objects[playStreamCommand->streamId - 1].get());
                                stream->play();
                                break;
                            }
                            case Command::Type::StopStream:
                            {
                                auto stopStreamCommand = static_cast<const StopStreamCommand*>(command.get());

                                Stream* stream = static_cast<Stream*>(objects[stopStreamCommand->streamId - 1].get());
                                stream->stop(stopStreamCommand->reset);
                                break;
                            }
                            case Command::Type::SetStreamOutput:
                            {
                                auto setStreamOutputCommand = static_cast<const SetStreamOutputCommand*>(command.get());

                                Stream* stream = static_cast<Stream*>(objects[setStreamOutputCommand->streamId - 1].get());
                                stream->setOutput(setStreamOutputCommand->busId ? static_cast<Bus*>(objects[setStreamOutputCommand->busId - 1].get()) : nullptr);
                                break;
                            }
                            case Command::Type::InitData:
                            {
                                auto initDataCommand = static_cast<InitDataCommand*>(command.get());

                                if (initDataCommand->dataId > objects.size())
                                    objects.resize(initDataCommand->dataId);

                                objects[initDataCommand->dataId - 1] = std::move(initDataCommand->data);
                                break;
                            }
                            case Command::Type::InitProcessor:
                            {
                                auto initProcessorCommand = static_cast<InitProcessorCommand*>(command.get());

                                if (initProcessorCommand->processorId > objects.size())
                                    objects.resize(initProcessorCommand->processorId);

                                objects[initProcessorCommand->processorId - 1] = std::move(initProcessorCommand->processor);
                                break;
                            }
                            case Command::Type::UpdateProcessor:
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
            }

            void Mixer::getSamples(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)
            {
                process();

                samples.resize(frames * channels);

                if (masterBus)
                {
                    Vector3F listenerPosition;
                    QuaternionF listenerRotation;

                    masterBus->getSamples(frames, channels, sampleRate, listenerPosition, listenerRotation, samples);
                }

                for (float& sample : samples)
                    sample = clamp(sample, -1.0F, 1.0F);
            }

            void Mixer::mixerMain()
            {
                Thread::setCurrentThreadName("Mixer");

                for (;;)
                {
                    break; // TODO: remove

                    process();

                    std::unique_lock<std::mutex> lock(bufferMutex);
                    bufferCondition.wait(lock);

                    if (masterBus)
                    {
                        //Vector3F listenerPosition;
                        //QuaternionF listenerRotation;

                        //masterBus->getData(frames, channels, sampleRate, listenerPosition, listenerRotation, samples);
                    }

                    //for (float& f : samples)
                    //    f = clamp(f, -1.0F, 1.0F);
                }
            }
        }
    } // namespace audio
} // namespace ouzel
