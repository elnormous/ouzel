// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "SoundOutput.hpp"
#include "SoundInput.hpp"

namespace ouzel
{
    namespace audio
    {
        SoundOutput::~SoundOutput()
        {
            for (SoundInput* input : inputs)
                input->output = nullptr;
        }

        void SoundOutput::addInput(SoundInput* input)
        {
            auto inputIterator = std::find(inputs.begin(), inputs.end(), input);

            if (inputIterator == inputs.end()) inputs.push_back(input);
        }

        void SoundOutput::removeInput(SoundInput* input)
        {
            auto inputIterator = std::find(inputs.begin(), inputs.end(), input);

            if (inputIterator != inputs.end()) inputs.erase(inputIterator);
        }
    } // namespace audio
} // namespace ouzel
