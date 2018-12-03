// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Mixer.hpp"
#include "Bus.hpp"
#include "math/MathUtils.hpp"

namespace ouzel
{
    namespace audio
    {
        void Mixer::addCommand(const Command& command)
        {
            std::unique_lock<std::mutex> lock(commandMutex);
            commandQueue.push(command);
            lock.unlock();
            commandConditionVariable.notify_all();
        }

        void Mixer::process()
        {
            Command command;
            for (;;)
            {
                std::unique_lock<std::mutex> lock(commandMutex);
                if (commandQueue.empty()) break;
                command = std::move(commandQueue.front());
                commandQueue.pop();
                lock.unlock();

                switch (command.type)
                {
                    case Command::Type::DELETE_OBJECT:
                        objects[command.objectId - 1].reset();
                        break;
                    case Command::Type::INIT_BUS:
                        objects[command.busId - 1].reset(new Bus());
                        break;
                    case Command::Type::ADD_LISTENER:
                        break;
                    case Command::Type::REMOVE_LISTENER:
                        break;
                    case Command::Type::ADD_PROCESSOR:
                        break;
                    case Command::Type::REMOVE_PROCESSOR:
                        break;
                    case Command::Type::INIT_LISTENER:
                        break;
                    case Command::Type::UPDATE_LISTENER:
                        break;
                    case Command::Type::INIT_PROCESSOR:
                    {
                        if (command.objectId > objects.size())
                            objects.resize(command.objectId);

                        objects[command.processorId - 1] = command.createFunction();
                        break;
                    }
                    case Command::Type::UPDATE_PROCESSOR:
                    {
                        command.updateFunction(static_cast<Processor*>(objects[command.processorId - 1].get()));
                        break;
                    }
                    case Command::Type::ADD_OUTPUT_BUS:
                    {
                        Bus* bus = static_cast<Bus*>(objects[command.busId - 1].get());
                        bus->setOutput(static_cast<Bus*>(objects[command.destinationBusId - 1].get()));
                        break;
                    }
                    case Command::Type::SET_MASTER_BUS:
                    {
                        masterBus = command.objectId ? static_cast<Bus*>(objects[command.busId - 1].get()) : nullptr;
                        break;
                    }
                    default:
                        throw std::runtime_error("Invalid command");
                }
            }
        }

        void Mixer::getData(uint32_t frames, uint32_t sampleRate, uint16_t channels, std::vector<float>& result)
        {
            if (masterBus)
            {
                uint16_t inputChannels = channels;
                uint32_t inputSampleRate = sampleRate;
                Vector3 inputPosition;

                masterBus->getData(result, inputChannels, inputSampleRate, inputPosition);
            }

            for (float& f : result)
                f = clamp(f, -1.0F, 1.0F);
        }
    } // namespace audio
} // namespace ouzel
