// Ouzel by Elviss Strazdins

#include "Voice.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Effects.hpp"
#include "Oscillator.hpp"
#include "SilenceSound.hpp"
#include "Sound.hpp"
#include "Source.hpp"
#include "WavePlayer.hpp"
#include "../core/Engine.hpp"

namespace ouzel::audio
{
    class VoiceSource: public mixer::Source
    {
    public:
        void play() override
        {
            if (source)
                source->play();
        }

        void stop(bool shouldReset) override
        {
            if (source)
                source->stop(shouldReset);
        }

        void getSamples(std::uint32_t frames, std::uint32_t channels, std::uint32_t sampleRate, std::vector<float>& samples) override
        {
            if (source)
                source->getSamples(frames, channels, sampleRate, samples);
        }

        std::unique_ptr<audio::Source> source;
    };

    Voice::Voice(Audio& initAudio):
        Node(initAudio),
        audio(initAudio)
    {
    }

    Voice::Voice(Audio& initAudio, const Cue& cue):
        Node(initAudio),
        audio(initAudio)
    {
        const auto& sourceDefinition = cue.getSourceDefinition();

        switch (sourceDefinition.type)
        {
            case SourceDefinition::Type::empty:
            {
                break;
            }
            case SourceDefinition::Type::parallel:
            {
                break;
            }
            case SourceDefinition::Type::random:
            {
                break;
            }
            case SourceDefinition::Type::sequence:
            {
                break;
            }
            case SourceDefinition::Type::oscillator:
            {
                std::unique_ptr<Oscillator> oscillator = std::make_unique<Oscillator>(initAudio,
                                                                                      sourceDefinition.frequency,
                                                                                      sourceDefinition.oscillatorType,
                                                                                      sourceDefinition.amplitude,
                                                                                      sourceDefinition.length);
                break;
            }
            case SourceDefinition::Type::silence:
            {
                std::unique_ptr<SilenceSound> silence = std::make_unique<SilenceSound>(initAudio,
                                                                                       sourceDefinition.length);
                break;
            }
            case SourceDefinition::Type::wavePlayer:
            {
                std::unique_ptr<WavePlayer> silence = std::make_unique<WavePlayer>(sourceDefinition.sound);
                break;
            }
        }

        std::vector<std::unique_ptr<Effect>> effects;

        for (const auto& effectDefinition : sourceDefinition.effectDefinitions)
        {
            switch (effectDefinition.type)
            {
                case EffectDefinition::Type::delay:
                    effects.push_back(std::make_unique<Delay>(initAudio, effectDefinition.delay));
                    break;
                case EffectDefinition::Type::gain:
                    effects.push_back(std::make_unique<Gain>(initAudio, effectDefinition.gain));
                    break;
                case EffectDefinition::Type::pitchScale:
                    effects.push_back(std::make_unique<PitchScale>(initAudio, effectDefinition.scale));
                    break;
                case EffectDefinition::Type::pitchShift:
                    effects.push_back(std::make_unique<PitchShift>(initAudio, effectDefinition.shift));
                    break;
                case EffectDefinition::Type::reverb:
                    effects.push_back(std::make_unique<Reverb>(initAudio, effectDefinition.delay, effectDefinition.decay));
                    break;
                case EffectDefinition::Type::lowPass:
                    effects.push_back(std::make_unique<LowPass>(initAudio));
                    break;
                case EffectDefinition::Type::highPass:
                    effects.push_back(std::make_unique<HighPass>(initAudio));
                    break;
            }
        }

        std::unique_ptr<mixer::Source> source = std::make_unique<VoiceSource>();

        audio.addCommand(std::make_unique<mixer::InitObjectCommand>(audio.getMixer().getObjectId(),
                                                                    std::move(source)));
    }

    Voice::Voice(Audio& initAudio, const Sound* initSound):
        Node(initAudio),
        audio(initAudio),
        streamId(audio.initStream(initSound->getSourceId()))
    {
        sound = initSound;
    }

    Voice::~Voice()
    {
        if (streamId)
            audio.deleteObject(streamId);
    }

    void Voice::play()
    {
        audio.addCommand(std::make_unique<mixer::PlayStreamCommand>(streamId));

        playing = true;

        auto startEvent = std::make_unique<SoundEvent>();
        startEvent->type = Event::Type::soundStart;
        startEvent->voice = this;
        engine->getEventDispatcher().postEvent(std::move(startEvent));

        // TODO: send PlayCommand
    }

    void Voice::pause()
    {
        audio.addCommand(std::make_unique<mixer::StopStreamCommand>(streamId, false));

        playing = false;

        // TODO: send StopCommand
    }

    void Voice::stop()
    {
        audio.addCommand(std::make_unique<mixer::StopStreamCommand>(streamId, true));

        playing = false;

        // TODO: send StopCommand
    }

    // executed on audio thread
    /*void Voice::onReset()
    {
        auto event = std::make_unique<SoundEvent>();
        event->type = Event::Type::soundReset;
        event->voice = this;
        engine->getEventDispatcher().postEvent(std::move(event));
    }

    // executed on audio thread
    void Voice::onStop()
    {
        playing = false;

        auto event = std::make_unique<SoundEvent>();
        event->type = Event::Type::soundFinish;
        event->voice = this;
        engine->getEventDispatcher().postEvent(std::move(event));
    }*/

    void Voice::setOutput(Mix* newOutput)
    {
        if (output) output->removeInput(this);
        output = newOutput;
        if (output) output->addInput(this);

        audio.addCommand(std::make_unique<mixer::SetStreamOutputCommand>(streamId,
                                                                         output ? output->getBusId() : 0));
    }
}
