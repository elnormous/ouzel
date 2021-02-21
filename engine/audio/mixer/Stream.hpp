// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_MIXER_STREAM_HPP
#define OUZEL_AUDIO_MIXER_STREAM_HPP

#include "Object.hpp"
#include "Bus.hpp"
#include "Data.hpp"

namespace ouzel::audio::mixer
{
    class Bus;
    class Data;

    class Stream: public Object
    {
        friend Bus;
    public:
        explicit Stream(Data& initData) noexcept:
            data(initData)
        {
        }

        ~Stream() override
        {
            if (output) output->removeInput(this);
        }

        Stream(const Stream&) = delete;
        Stream& operator=(const Object&) = delete;

        Stream(Stream&&) = delete;
        Stream& operator=(Stream&&) = delete;

        auto& getData() const noexcept { return data; }

        void setOutput(Bus* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            if (output) output->addInput(this);
        }

        auto isPlaying() const noexcept { return playing; }
        void play() { playing = true; }

        void stop(bool shouldReset)
        {
            playing = false;
            if (shouldReset) reset();
        }

        virtual void reset() = 0;

        virtual void generateSamples(std::uint32_t frames, std::vector<float>& samples) = 0;

    protected:
        Data& data;
        Bus* output = nullptr;
        bool playing = false;
    };
}

#endif // OUZEL_AUDIO_MIXER_STREAM_HPP
