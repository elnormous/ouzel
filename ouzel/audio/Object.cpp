// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Object.hpp"

namespace ouzel
{
    namespace audio
    {
        Object::~Object()
        {
            for (Object* outputObject : outputObjects)
                outputObject->removeInputObject(this);
        }

        void Object::process(std::vector<float>& samples, uint16_t& channels,
                           uint32_t& sampleRate, Vector3& position)
        {
            buffer.resize(samples.size());
            std::fill(buffer.begin(), buffer.end(), 0.0F);

            for (Object* inputObject : inputObjects)
            {
                inputObject->process(buffer, channels, sampleRate, position);

                for (uint32_t i = 0; i < samples.size(); ++i)
                    samples[i] += buffer[i];
            }
        }

        void Object::addOutputObject(Object* node)
        {
            auto i = std::find(outputObjects.begin(), outputObjects.end(), node);
            if (i == outputObjects.end())
            {
                node->addInputObject(this);
                outputObjects.push_back(node);
            }
        }

        void Object::removeOutputObject(Object* node)
        {
            auto i = std::find(outputObjects.begin(), outputObjects.end(), node);
            if (i != outputObjects.end())
            {
                node->removeInputObject(this);
                outputObjects.erase(i);
            }
        }

        void Object::addInputObject(Object* node)
        {
            auto i = std::find(inputObjects.begin(), inputObjects.end(), node);
            if (i == inputObjects.end())
                inputObjects.push_back(node);
        }

        void Object::removeInputObject(Object* node)
        {
            auto i = std::find(inputObjects.begin(), inputObjects.end(), node);
            if (i != inputObjects.end())
                inputObjects.erase(i);
        }
    } // namespace audio
} // namespace ouzel
