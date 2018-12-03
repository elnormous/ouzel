// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_GROUPBUS_HPP
#define OUZEL_AUDIO_GROUPBUS_HPP

#include "audio/Bus.hpp"

namespace ouzel
{
    namespace audio
    {
        class GroupBus final: public Bus
        {
        public:
            GroupBus();
            GroupBus(const GroupBus&) = delete;
            GroupBus& operator=(const GroupBus&) = delete;

            GroupBus(GroupBus&&) = delete;
            GroupBus& operator=(GroupBus&&) = delete;

            void setOutput(Bus* newOutput);

        private:
            Bus* output = nullptr;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_GROUPBUS_HPP
