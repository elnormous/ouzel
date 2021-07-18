// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_MIXER_OBJECT_HPP
#define OUZEL_AUDIO_MIXER_OBJECT_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include "Source.hpp"
#include "../../math/Quaternion.hpp"
#include "../../math/Vector.hpp"

namespace ouzel::audio::mixer
{
    class Object
    {
    public:
        Object() noexcept = default;
        explicit Object(std::unique_ptr<Source> initSource) noexcept:
            source{std::move(initSource)}
        {
        }

        virtual ~Object()
        {
            if (parent)
                parent->removeChild(*this);
        }

        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;

        Object(Object&&) = delete;
        Object& operator=(Object&&) = delete;

        void addChild(Object& child)
        {
            if (child.parent != this)
            {
                if (child.parent)
                    child.parent->removeChild(child);

                if (std::find(children.begin(), children.end(), &child) == children.end())
                {
                    child.parent = this;
                    children.push_back(&child);
                }
            }
        }

        void removeChild(Object& child)
        {
            if (child.parent == this)
                if (const auto i = std::find(children.begin(), children.end(), &child); i != children.end())
                {
                    child.parent = this;
                    children.erase(i);
                }
        }

        void play()
        {
            if (source)
                source->play();
        }

        void stop(bool reset)
        {
            if (source)
                source->stop(reset);
        }

        // TODO: make non-virtual
        virtual void getSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate, std::vector<float>& samples)
        {
            for (auto child : children)
                child->getSamples(frames, channels, sampleRate, samples);

            if (source)
                source->getSamples(frames, channels, sampleRate, samples);
        }

    protected:
        Object* parent = nullptr;
        std::vector<Object*> children;
        std::unique_ptr<Source> source;
    };
}

#endif // OUZEL_AUDIO_MIXER_OBJECT_HPP
