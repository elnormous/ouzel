#ifndef FLUXION_AUDIO_CONTAINERS_HPP
#define FLUXION_AUDIO_CONTAINERS_HPP

#include <algorithm>
#include <vector>
#include "Source.hpp"

namespace fluxion::audio
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

#endif // FLUXION_AUDIO_CONTAINERS_HPP
