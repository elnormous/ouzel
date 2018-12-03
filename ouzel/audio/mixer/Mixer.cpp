// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Mixer.hpp"
#include "Bus.hpp"
#include "Source.hpp"
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
                    {
                        if (command.busId > objects.size())
                            objects.resize(command.busId);

                        objects[command.busId - 1].reset(new Bus());
                        break;
                    }
                    case Command::Type::SET_BUS_OUTPUT:
                    {
                        Bus* bus = static_cast<Bus*>(objects[command.busId - 1].get());
                        bus->setOutput(command.outputBusId ? static_cast<Bus*>(objects[command.outputBusId - 1].get()) : nullptr);
                        break;
                    }
                    case Command::Type::INIT_SOURCE:
                    {
                        if (command.sourceId > objects.size())
                            objects.resize(command.sourceId);

                        objects[command.sourceId - 1].reset(new Source());
                        break;
                    }
                    case Command::Type::SET_SOURCE_OUTPUT:
                    {
                        Source* source = static_cast<Source*>(objects[command.sourceId - 1].get());
                        source->setOutput(command.outputBusId ? static_cast<Bus*>(objects[command.outputBusId - 1].get()) : nullptr);
                        break;
                    }
                    case Command::Type::INIT_PROCESSOR:
                    {
                        if (command.processorId > objects.size())
                            objects.resize(command.processorId);

                        objects[command.processorId - 1] = command.createFunction();
                        break;
                    }
                    case Command::Type::SET_PROCESSOR_BUS:
                    {
                        Processor* processor = static_cast<Processor*>(objects[command.processorId - 1].get());
                        processor->setBus(command.busId ? static_cast<Bus*>(objects[command.busId - 1].get()) : nullptr);
                        break;
                    }
                    case Command::Type::UPDATE_PROCESSOR:
                    {
                        Processor* processor = static_cast<Processor*>(objects[command.processorId - 1].get());
                        command.updateFunction(processor);
                        break;
                    }
                    case Command::Type::SET_MASTER_BUS:
                    {
                        masterBus = command.busId ? static_cast<Bus*>(objects[command.busId - 1].get()) : nullptr;
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
