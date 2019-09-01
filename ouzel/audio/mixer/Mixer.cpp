// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Mixer.hpp"
#include "Bus.hpp"
#include "Data.hpp"
#include "Source.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            Mixer::Mixer(uint32_t initBufferSize,
                         uint32_t initChannels,
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
                                //object->addChild(*child);
                                break;
                            }
                            case Command::Type::RemoveChild:
                            {
                                auto removeChildCommand = static_cast<const RemoveChildCommand*>(command.get());
                                Object* object = objects[removeChildCommand->objectId - 1].get();
                                Object* child = objects[removeChildCommand->objectId - 1].get();
                                //object->removeChild(*child);
                                break;
                            }
                            case Command::Type::Play:
                            {
                                auto playCommand = static_cast<const PlayCommand*>(command.get());
                                Object* object = objects[playCommand->objectId - 1].get();
                                //object->play();
                                break;
                            }
                            case Command::Type::Stop:
                            {
                                auto stopCommand = static_cast<const StopCommand*>(command.get());
                                Object* object = objects[stopCommand->objectId - 1].get();
                                //object->stop(stopCommand->reset);
                                break;
                            }
                            case Command::Type::InitBus:
                            {
                                auto initBusCommand = static_cast<InitBusCommand*>(command.get());

                                if (initBusCommand->busId > objects.size())
                                    objects.resize(initBusCommand->busId);

                                objects[initBusCommand->busId - 1] = std::make_unique<Bus>(std::move(initBusCommand->processor));
                                break;
                            }
                            case Command::Type::SetBusOutput:
                            {
                                auto setBusOutputCommand = static_cast<const SetBusOutputCommand*>(command.get());

                                Bus* bus = static_cast<Bus*>(objects[setBusOutputCommand->busId - 1].get());
                                bus->setOutput(setBusOutputCommand->outputBusId ? static_cast<Bus*>(objects[setBusOutputCommand->outputBusId - 1].get()) : nullptr);
                                break;
                            }
                            case Command::Type::SetMasterBus:
                            {
                                auto setMasterBusCommand = static_cast<const SetMasterBusCommand*>(command.get());

                                masterBus = setMasterBusCommand->busId ? static_cast<Bus*>(objects[setMasterBusCommand->busId - 1].get()) : nullptr;
                                break;
                            }
                            case Command::Type::InitSource:
                            {
                                auto initSourceCommand = static_cast<InitSourceCommand*>(command.get());

                                if (initSourceCommand->sourceId > objects.size())
                                    objects.resize(initSourceCommand->sourceId);

                                objects[initSourceCommand->sourceId - 1] = std::make_unique<Source>(std::move(initSourceCommand->emitter));
                                break;
                            }
                            case Command::Type::PlaySource:
                            {
                                auto playSourceCommand = static_cast<const PlaySourceCommand*>(command.get());

                                Source* source = static_cast<Source*>(objects[playSourceCommand->sourceId - 1].get());
                                source->play();
                                break;
                            }
                            case Command::Type::StopSource:
                            {
                                auto stopSourceCommand = static_cast<const StopSourceCommand*>(command.get());

                                Source* source = static_cast<Source*>(objects[stopSourceCommand->sourceId - 1].get());
                                source->stop(stopSourceCommand->reset);
                                break;
                            }
                            case Command::Type::SetSourceOutput:
                            {
                                auto setSourceOutputCommand = static_cast<const SetSourceOutputCommand*>(command.get());

                                Source* source = static_cast<Source*>(objects[setSourceOutputCommand->sourceId - 1].get());
                                //source->setOutput(setSourceOutputCommand->busId ? static_cast<Bus*>(objects[setSourceOutputCommand->busId - 1].get()) : nullptr);
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
                            default:
                                throw std::runtime_error("Invalid command");
                        }
                    }
                }
            }

            void Mixer::getSamples(uint32_t frames, uint32_t channels, uint32_t sampleRate, std::vector<float>& samples)
            {
                process();

                samples.resize(frames * channels);

                if (masterBus)
                    masterBus->getSamples(frames, channels, sampleRate, samples);

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
