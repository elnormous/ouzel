// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Voice.hpp"
#include "Audio.hpp"
#include "AudioDevice.hpp"
#include "Effects.hpp"
#include "Oscillator.hpp"
#include "SilenceSound.hpp"
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
                    std::unique_ptr<Oscillator> oscillator = std::make_unique<Oscillator>(initAudio,
                                                                                          sourceDefinition.frequency,
                                                                                          sourceDefinition.oscillatorType,
                                                                                          sourceDefinition.amplitude,
                                                                                          sourceDefinition.length);
                    break;
                }
                case SourceDefinition::Type::Silence:
                {
                    std::unique_ptr<SilenceSound> silence = std::make_unique<SilenceSound>(initAudio,
                                                                                           sourceDefinition.length);
                    break;
                }
                case SourceDefinition::Type::WavePlayer:
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
                    case EffectDefinition::Type::Delay:
                        effects.push_back(std::make_unique<Delay>(initAudio, effectDefinition.delay));
                        break;
                    case EffectDefinition::Type::Gain:
                        effects.push_back(std::make_unique<Gain>(initAudio, effectDefinition.gain));
                        break;
                    case EffectDefinition::Type::PitchScale:
                        effects.push_back(std::make_unique<PitchScale>(initAudio, effectDefinition.scale));
                        break;
                    case EffectDefinition::Type::PitchShift:
                        effects.push_back(std::make_unique<PitchShift>(initAudio, effectDefinition.shift));
                        break;
                    case EffectDefinition::Type::Reverb:
                        effects.push_back(std::make_unique<Reverb>(initAudio, effectDefinition.delay, effectDefinition.decay));
                        break;
                    case EffectDefinition::Type::LowPass:
                        effects.push_back(std::make_unique<LowPass>(initAudio));
                        break;
                    case EffectDefinition::Type::HighPass:
                        effects.push_back(std::make_unique<HighPass>(initAudio));
                        break;
                }
            }

            std::unique_ptr<mixer::Emitter> emitter = std::make_unique<VoiceEmitter>();

            //audio.addCommand(std::make_unique<mixer::InitObjectCommand>(audio.getMixer().getObjectId(),
            //                                                            std::move(source)));
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
            startEvent->type = Event::Type::SoundStart;
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

            audio.addCommand(std::make_unique<mixer::SetStreamOutputCommand>(streamId,
                                                                             output ? output->getBusId() : 0));
        }
    } // namespace audio
} // namespace ouzel
