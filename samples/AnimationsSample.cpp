// Ouzel by Elviss Strazdins

#include "core/Engine.hpp"
#include "AnimationsSample.hpp"
#include "MainMenu.hpp"

using namespace ouzel;
using namespace input;

namespace samples
{
    AnimationsSample::AnimationsSample():
        backButton("button.png", "button_selected.png", "button_down.png", "", "Back", "Arial", 1.0F, Color::black(), Color::black(), Color::black())
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
            if (event.type == Event::Type::actorClick && event.actor == &backButton)
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());

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

        camera.setClearColorBuffer(true);
        camera.setClearColor(ouzel::Color(64, 0, 0));
        camera.setScaleMode(scene::Camera::ScaleMode::showAll);
        camera.setTargetContentSize(ouzel::Size<float, 2>{800.0F, 600.0F});
        cameraActor.addComponent(camera);
        layer.addChild(cameraActor);
        addLayer(layer);

        shapeDrawable.rectangle(ouzel::Rect<float>{100.0F, 100.0F}, Color{0, 128, 128, 255}, true);
        shapeDrawable.rectangle(ouzel::Rect<float>{100.0F, 100.0F}, Color::white(), false, 2.0F);
        shapeDrawable.line(Vector<float, 2>(0.0F, 0.0F), Vector<float, 2>(50.0F, 50.0F), Color::cyan(), 2.0F);

        shapeDrawable.curve({Vector<float, 2>{50.0F, 50.0F},
                             Vector<float, 2>{100.0F, 50.0F},
                             Vector<float, 2>{50.0F, 0.0F},
                             Vector<float, 2>{100.0F, 0.0F}},
                            Color::yellow());

        shapeDrawable.circle(Vector<float, 2>{25.0F, 75.0F}, 20.0F, Color::blue(), true);
        shapeDrawable.circle(Vector<float, 2>{25.0F, 75.0F}, 20.0F, Color::white(), false);
        shapeDrawable.circle(Vector<float, 2>{75.0F, 75.0F}, 20.0F, Color::blue(), false, 16, 4.0F);

        shapeDrawable.polygon({Vector<float, 2>{15.0F, 75.0F},
                               Vector<float, 2>{25.0F, 75.0F},
                               Vector<float, 2>{25.0F, 55.0F}},
                              Color("#ff0000"), false);

        drawActor.addComponent(shapeDrawable);
        drawActor.setPosition(Vector<float, 2>{-300.0F, 0.0F});
        layer.addChild(drawActor);

        shake = std::make_unique<scene::Shake>(10.0F, Vector<float, 3>{10.0F, 20.0F, 0.0F}, 20.0F);
        drawActor.addComponent(*shake);
        shake->start();

        witchSprite.init("witch.png");

        witch.setPosition(Vector<float, 2>{200.0F, 0.0F});
        witch.addComponent(witchSprite);
        layer.addChild(witch);

        witchScale = std::make_unique<scene::Scale>(2.0F, Vector<float, 3>{0.1F, 0.1F, 0.0F}, false);
        witchFade = std::make_unique<scene::Fade>(2.0F, 0.4F);

        std::vector<scene::Animator*> parallel = {
            witchScale.get(),
            witchFade.get()
        };

        witchRotate = std::make_unique<scene::Rotate>(1.0F, Vector<float, 3>{0.0F, 0.0F, tau<float>}, false);

        witchRepeat = std::make_unique<scene::Repeat>(*witchRotate, 3);
        witchParallel = std::make_unique<scene::Parallel>(parallel);

        std::vector<scene::Animator*> sequence = {
            witchRepeat.get(),
            witchParallel.get()
        };

        witchSequence = std::make_unique<scene::Sequence>(sequence);

        witch.addComponent(*witchSequence);
        witchSequence->start();

        ballSprite.init("ball.png");

        ball.addComponent(ballSprite);
        layer.addChild(ball);

        ballDelay = std::make_unique<scene::Animator>(1.0F);
        ballMove = std::make_unique<scene::Move>(2.0F, Vector<float, 3>{0.0F, -240.0F, 0.0F}, false);
        ballEase = std::make_unique<scene::Ease>(*ballMove, scene::Ease::Mode::easeOut, scene::Ease::Func::bounce);

        std::vector<scene::Animator*> sequence2 = {
            ballDelay.get(),
            ballEase.get()
        };

        ballSequence = std::make_unique<scene::Sequence>(sequence2);

        ball.addComponent(*ballSequence);
        ballSequence->start();

        guiCamera.setScaleMode(scene::Camera::ScaleMode::showAll);
        guiCamera.setTargetContentSize(ouzel::Size<float, 2>{800.0F, 600.0F});
        guiCameraActor.addComponent(guiCamera);
        guiLayer.addChild(guiCameraActor);
        addLayer(guiLayer);

        guiLayer.addChild(menu);

        backButton.setPosition(Vector<float, 2>{-200.0F, -200.0F});
        menu.addWidget(backButton);
    }
}

