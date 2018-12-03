// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Mix.hpp"
#include "Audio.hpp"
#include "Filter.hpp"
#include "Listener.hpp"

namespace ouzel
{
    namespace audio
    {
        Mix::Mix(Audio& initAudio):
            audio(initAudio),
            busId(audio.initBus())
        {
        }

        Mix::~Mix()
        {
            for (Filter* filter : filters)
                filter->mix = nullptr;

            for (Listener* listener : listeners)
                listener->mix = nullptr;

            if (busId) audio.deleteObject(busId);
        }

        void Mix::addFilter(Filter* filter)
        {
            auto i = std::find(filters.begin(), filters.end(), filter);

            if (i == filters.end())
            {
                if (filter->mix) filter->mix->removeFilter(filter);
                filter->mix = this;
                filters.push_back(filter);

                Mixer::Command command(Mixer::Command::Type::ADD_PROCESSOR);
                command.busId = busId;
                command.processorId = filter->getProcessorId();
                audio.getMixer().addCommand(command);
            }
        }

        void Mix::removeFilter(Filter* filter)
        {
            auto i = std::find(filters.begin(), filters.end(), filter);

            if (i != filters.end())
            {
                Mixer::Command command(Mixer::Command::Type::REMOVE_PROCESSOR);
                command.busId = busId;
                command.processorId = filter->getProcessorId();
                audio.getMixer().addCommand(command);

                filter->mix = nullptr;
                filters.erase(i);
            }
        }

        void Mix::addListener(Listener* listener)
        {
            auto i = std::find(listeners.begin(), listeners.end(), listener);

            if (i == listeners.end())
            {
                if (listener->mix) listener->mix->removeListener(listener);
                listener->mix = this;
                listeners.push_back(listener);
            }
        }

        void Mix::removeListener(Listener* listener)
        {
            auto i = std::find(listeners.begin(), listeners.end(), listener);

            if (i != listeners.end())
            {
                listener->mix = nullptr;
                listeners.erase(i);
            }
        }
    } // namespace audio
} // namespace ouzel
