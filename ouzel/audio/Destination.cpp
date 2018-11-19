// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Destination.hpp"
#include "Source.hpp"

namespace ouzel
{
    namespace audio
    {
        Destination::~Destination()
        {
            for (Source* input : inputs)
                input->output = nullptr;
        }

        void Destination::addInput(Source* input)
        {
            auto inputIterator = std::find(inputs.begin(), inputs.end(), input);

            if (inputIterator == inputs.end()) inputs.push_back(input);
        }

        void Destination::removeInput(Source* input)
        {
            auto inputIterator = std::find(inputs.begin(), inputs.end(), input);

            if (inputIterator != inputs.end()) inputs.erase(inputIterator);
        }
    } // namespace audio
} // namespace ouzel
