// Ouzel by Elviss Strazdins

#include "core/Engine.hpp"
#include "SoundSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace audio;
using namespace input;
using namespace math;

namespace samples
{
    SoundSample::SoundSample():
        submix{*engine->getAudio()},
        listener{*engine->getAudio()},
        soundGain{*engine->getAudio()},
        test8BitSubmix{*engine->getAudio()},
        test8BitVoice{*engine->getAudio(), engine->getCache().getSound("8-bit.wav")},
        test8BitPitch{*engine->getAudio()},
        test24BitSubmix{*engine->getAudio()},
        test24BitVoice{*engine->getAudio(), engine->getCache().getSound("24-bit.wav")},
        test24BitPitch{*engine->getAudio()},
        jumpSubmix{*engine->getAudio()},
        jumpVoice{*engine->getAudio(), engine->getCache().getSound("jump.wav")},
        jumpPanner{*engine->getAudio()},
        jumpReverb{*engine->getAudio()},
        ambientVoice{*engine->getAudio(), engine->getCache().getSound("ambient.wav")},
        music{*engine->getAudio(), engine->getCache().getSound("music.ogg")},
        tone{*engine->getAudio(), 200.0F, Oscillator::Type::sine, 0.5F, 1.0F},
        toneVoice{*engine->getAudio(), &tone},
        backButton{"button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, Color::black(), Color::black(), Color::black()},
        test8BitButton{"button.png", "button_selected.png", "button_down.png", "", "8-bit", "Arial", 1.0F, Color::black(), Color::black(), Color::black()},
        test24BitButton{"button.png", "button_selected.png", "button_down.png", "", "24-bit", "Arial", 1.0F, Color::black(), Color::black(), Color::black()},
        jumpButton{"button.png", "button_selected.png", "button_down.png", "", "Jump", "Arial", 1.0F, Color::black(), Color::black(), Color::black()},
        ambientButton{"button.png", "button_selected.png", "button_down.png", "", "Ambient", "Arial", 1.0F, Color::black(), Color::black(), Color::black()},
        musicButton{"button.png", "button_selected.png", "button_down.png", "", "Music", "Arial", 1.0F, Color::black(), Color::black(), Color::black()},
        toneButton{"button.png", "button_selected.png", "button_down.png", "", "Tone", "Arial", 1.0F, Color::black(), Color::black(), Color::black()}
    {
        handler.gamepadHandler = [](const GamepadEvent& event) {
            if (event.type == Event::Type::gamepadButtonChange)
            {
                if (event.pressed &&
                    event.button == Gamepad::Button::faceRight)
                    engine->getSceneManager().setScene(std::make_unique<MainMenu>());
            }

            return false;
        };

        handler.uiHandler = [this](const UIEvent& event) {
            if (event.type == Event::Type::actorClick)
            {
                if (event.actor == &backButton)
                    engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                else if (event.actor == &test8BitButton)
                    test8BitVoice.isPlaying() ? test8BitVoice.pause() : test8BitVoice.play();
                else if (event.actor == &test24BitButton)
                    test24BitVoice.isPlaying() ? test24BitVoice.pause() : test24BitVoice.play();
                else if (event.actor == &jumpButton)
                    jumpVoice.isPlaying() ? jumpVoice.pause() : jumpVoice.play();
                else if (event.actor == &ambientButton)
                    ambientVoice.isPlaying() ? ambientVoice.pause() : ambientVoice.play();
                else if (event.actor == &musicButton)
                    music.isPlaying() ? music.pause() : music.play();
                else if (event.actor == &toneButton)
                    toneVoice.isPlaying() ? toneVoice.pause() : toneVoice.play();
            }

            return false;
        };

        handler.keyboardHandler = [](const KeyboardEvent& event) {
            if (event.type == Event::Type::keyboardKeyPress)
            {
                switch (event.key)
                {
                    case Keyboard::Key::escape:
                    case Keyboard::Key::menu:
                    case Keyboard::Key::back:
                        engine->getSceneManager().setScene(std::make_unique<MainMenu>());
                        return true;
                    default:
                        break;
                }
            }
            else if (event.type == Event::Type::keyboardKeyRelease)
            {
                switch (event.key)
                {
                    case Keyboard::Key::escape:
                    case Keyboard::Key::menu:
                    case Keyboard::Key::back:
                        return true;
                    default:
                        break;
                }
            }

            return false;
        };

        engine->getEventDispatcher().addEventHandler(handler);

        submix.setOutput(&engine->getAudio()->getMasterMix());

        listener.setMix(&submix);
        submix.addEffect(&soundGain);
        soundGain.setGain(1.0F);

        test8BitPitch.setScale(2.0F);
        test8BitVoice.setOutput(&test8BitSubmix);
        test8BitSubmix.addEffect(&test8BitPitch);
        test8BitSubmix.setOutput(&submix);

        test24BitPitch.setScale(0.5F);
        test24BitVoice.setOutput(&test24BitSubmix);
        test24BitSubmix.addEffect(&test24BitPitch);
        test24BitSubmix.setOutput(&submix);

        layer.addChild(soundActor);
        soundActor.addComponent(jumpPanner);
        soundActor.setPosition(Vector<float, 3>{8.0F, 0.0F, 10.0F});
        jumpVoice.setOutput(&jumpSubmix);
        jumpSubmix.addEffect(&jumpPanner);
        jumpSubmix.addEffect(&jumpReverb);
        jumpSubmix.setOutput(&submix);

        ambientVoice.setOutput(&submix);
        music.setOutput(&submix);
        toneVoice.setOutput(&submix);

        camera.setClearColorBuffer(true);
        camera.setClearColor(math::Color{64, 0, 0});
        camera.setScaleMode(scene::Camera::ScaleMode::showAll);
        camera.setTargetContentSize(math::Size<float, 2>{800.0F, 600.0F});
        cameraActor.addComponent(camera);
        layer.addChild(cameraActor);
        addLayer(layer);

        layer.addChild(menu);

        test8BitButton.setPosition(Vector<float, 2>{0.0F, 80.0F});
        menu.addWidget(test8BitButton);

        test24BitButton.setPosition(Vector<float, 2>{0.0F, 40.0F});
        menu.addWidget(test24BitButton);

        jumpButton.setPosition(Vector<float, 2>{0.0F, 0.0F});
        menu.addWidget(jumpButton);

        ambientButton.setPosition(Vector<float, 2>{0.0F, -40.0F});
        menu.addWidget(ambientButton);

        musicButton.setPosition(Vector<float, 2>{0.0F, -80.0F});
        menu.addWidget(musicButton);

        toneButton.setPosition(Vector<float, 2>{0.0F, -120.0F});
        menu.addWidget(toneButton);

        backButton.setPosition(Vector<float, 2>{-200.0F, -200.0F});
        menu.addWidget(backButton);
    }
}
