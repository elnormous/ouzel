// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Voice.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Effects.hpp"
#include "Oscillator.hpp"
#include "Silence.hpp"
#include "Sound.hpp"
#include "Source.hpp"
#include "WavePlayer.hpp"
#include "core/Engine.hpp"
#include "smbPitchShift.hpp"

namespace ouzel
{
    namespace audio
    {
        class VoiceEmitter: public mixer::Emitter
        {
        public:
            VoiceEmitter() = default;
            VoiceEmitter(std::unique_ptr<audio::Source> initSource):
                source(std::move(initSource))
            {
            }

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

            void getSamples(uint32_t frames, uint32_t channels, uint32_t sampleRate, std::vector<float>& samples) override
            {
                if (source)
                    source->getSamples(frames, channels, sampleRate, samples);
            }

        private:
            std::unique_ptr<audio::Source> source;
        };

        Voice::Voice(Audio& initAudio):
            audio(initAudio)
        {
        }

        static std::unique_ptr<Source> createSource(Audio& audio,
                                                    const SourceDefinition& sourceDefinition)
        {
            std::unique_ptr<Source> result;

            switch (sourceDefinition.type)
            {
                case SourceDefinition::Type::Empty:
                {
                    break;
                }
                case SourceDefinition::Type::Parallel:
                {
                    break;
                }
                case SourceDefinition::Type::Random:
                {
                    break;
                }
                case SourceDefinition::Type::Sequence:
                {
                    break;
                }
                case SourceDefinition::Type::Oscillator:
                {
                    result = std::make_unique<Oscillator>(audio,
                                                          sourceDefinition.frequency,
                                                          sourceDefinition.oscillatorType,
                                                          sourceDefinition.amplitude,
                                                          sourceDefinition.length);
                    break;
                }
                case SourceDefinition::Type::Silence:
                {
                    result = std::make_unique<Silence>(sourceDefinition.length);
                    break;
                }
                case SourceDefinition::Type::WavePlayer:
                {
                    //result = std::make_unique<WavePlayer>(sourceDefinition.sound);
                    break;
                }
            }

            std::vector<std::unique_ptr<Effect>> effects;

            for (const auto& effectDefinition : sourceDefinition.effectDefinitions)
            {
                switch (effectDefinition.type)
                {
                    case EffectDefinition::Type::Delay:
                        effects.push_back(std::make_unique<Delay>(audio, effectDefinition.delay));
                        break;
                    case EffectDefinition::Type::Gain:
                        effects.push_back(std::make_unique<Gain>(audio, effectDefinition.gain));
                        break;
                    case EffectDefinition::Type::PitchScale:
                        effects.push_back(std::make_unique<PitchScale>(audio, effectDefinition.scale));
                        break;
                    case EffectDefinition::Type::PitchShift:
                        effects.push_back(std::make_unique<PitchShift>(audio, effectDefinition.shift));
                        break;
                    case EffectDefinition::Type::Reverb:
                        effects.push_back(std::make_unique<Reverb>(audio, effectDefinition.delay, effectDefinition.decay));
                        break;
                    case EffectDefinition::Type::LowPass:
                        effects.push_back(std::make_unique<LowPass>(audio));
                        break;
                    case EffectDefinition::Type::HighPass:
                        effects.push_back(std::make_unique<HighPass>(audio));
                        break;
                }
            }

            return result;
        }

        Voice::Voice(Audio& initAudio, const Cue& cue):
            audio(initAudio)
        {
            const auto& sourceDefinition = cue.getSourceDefinition();

            auto emitter = std::make_unique<VoiceEmitter>(createSource(initAudio,
                                                                       sourceDefinition));
            sourceId = audio.initSource(std::move(emitter));
        }

        Voice::Voice(Audio& initAudio, const Sound& initSound):
            audio(initAudio),
            sourceId(audio.initSource(std::make_unique<VoiceEmitter>()))
        {
            sound = &initSound;
        }

        Voice::~Voice()
        {
            if (sourceId)
                audio.deleteObject(sourceId);
        }

        void Voice::play()
        {
            audio.addCommand(std::make_unique<mixer::PlaySourceCommand>(sourceId));

            playing = true;

            auto startEvent = std::make_unique<SoundEvent>();
            startEvent->type = Event::Type::SoundStart;
            startEvent->voice = this;
            engine->getEventDispatcher().postEvent(std::move(startEvent));

            // TODO: send PlayCommand
        }

        void Voice::pause()
        {
            audio.addCommand(std::make_unique<mixer::StopSourceCommand>(sourceId, false));

            playing = false;

            // TODO: send StopCommand
        }

        void Voice::stop()
        {
            audio.addCommand(std::make_unique<mixer::StopSourceCommand>(sourceId, true));

            playing = false;

            // TODO: send StopCommand
        }

        // executed on audio thread
        /*void Voice::onReset()
        {
            auto event = std::make_unique<SoundEvent>();
            event->type = Event::Type::SoundReset;
            event->voice = this;
            engine->getEventDispatcher().postEvent(std::move(event));
        }

        // executed on audio thread
        void Voice::onStop()
        {
            playing = false;

            auto event = std::make_unique<SoundEvent>();
            event->type = Event::Type::SoundFinish;
            event->voice = this;
            engine->getEventDispatcher().postEvent(std::move(event));
        }*/

        void Voice::setOutput(Mix* newOutput)
        {
            if (output) output->removeInput(this);
            output = newOutput;
            if (output) output->addInput(this);

            audio.addCommand(std::make_unique<mixer::SetSourceOutputCommand>(sourceId,
                                                                             output ? output->getBusId() : 0));
        }
    } // namespace audio
} // namespace ouzel
