// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_CONTAINERS_HPP
#define OUZEL_AUDIO_CONTAINERS_HPP

#include <algorithm>
#include <vector>
#include "Source.hpp"

namespace ouzel::audio
{
    class Container: public Source
    {
    public:
    };

    class Parallel final: public Container
    {
    public:
        void getSamples(std::uint32_t, std::uint32_t, std::uint32_t, std::vector<float>&) override
        {
        }
    };

    class Random final: public Container
    {
    public:
        void getSamples(std::uint32_t, std::uint32_t, std::uint32_t, std::vector<float>&) override
        {
        }
    };

    class Sequence final: public Container
    {
    public:
        void getSamples(std::uint32_t, std::uint32_t, std::uint32_t, std::vector<float>&) override
        {
        }
    };
}

#endif // OUZEL_AUDIO_CONTAINERS_HPP
