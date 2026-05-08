#include "core/Engine.hpp"
#include "ImGuiSample.hpp"
#include "ImGuiHost.hpp"
#include "MainMenu.hpp"

using namespace fluxion;
using namespace input;
using namespace math;

namespace samples
{
    namespace
    {
        constexpr Color buttonText{20, 0, 0, 255};

        std::string buttonLabel(const char* prefix, bool on)
        {
            return std::string{prefix} + (on ? ": on" : ": off");
        }
    }

    ImGuiSample::ImGuiSample():
        smokeTestButton{"button.png", "button_selected.png", "button_down.png", "",
                        buttonLabel("Smoke test", imguiHost().showSmokeTest.load()),
                        "Arial", 1.0F, buttonText, blackColor, blackColor},
        textRefButton{"button.png", "button_selected.png", "button_down.png", "",
                      buttonLabel("Text reference", imguiHost().showTextRef.load()),
                      "Arial", 1.0F, buttonText, blackColor, blackColor},
        demoButton{"button.png", "button_selected.png", "button_down.png", "",
                   buttonLabel("Demo window", imguiHost().showDemo.load()),
                   "Arial", 1.0F, buttonText, blackColor, blackColor},
        enableImGuiButton{"button.png", "button_selected.png", "button_down.png", "",
                          (imguiHost().context && imguiHost().context->isEnabled())
                              ? "Disable ImGui" : "Enable ImGui",
                          "Arial", 1.0F, buttonText, blackColor, blackColor},
        backButton{"button.png", "button_selected.png", "button_down.png", "",
                   "Back", "Arial", 1.0F, buttonText, blackColor, blackColor},
        statusLabel{"Toggle ImGui windows from the engine's native UI",
                    "Arial", 1.0F, whiteColor, Vector<float, 2>{0.5F, 0.5F}}
    {
        handler.uiHandler = [this](const UIEvent& event) {
            if (event.type != Event::Type::actorClick) return false;

            auto& host = imguiHost();

            if (event.actor == &smokeTestButton)
            {
                const bool next = !host.showSmokeTest.load();
                host.showSmokeTest.store(next);
                refreshLabels();
            }
            else if (event.actor == &textRefButton)
            {
                const bool next = !host.showTextRef.load();
                host.showTextRef.store(next);
                refreshLabels();
            }
            else if (event.actor == &demoButton)
            {
                const bool next = !host.showDemo.load();
                host.showDemo.store(next);
                refreshLabels();
            }
            else if (event.actor == &enableImGuiButton)
            {
                if (host.context)
                {
                    host.context->setEnabled(!host.context->isEnabled());
                    refreshLabels();
                }
            }
            else if (event.actor == &backButton)
            {
                engine->getSceneManager().setScene(std::make_unique<MainMenu>());
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

        camera.setClearColorBuffer(true);
        camera.setClearColor(Color{32, 16, 64});
        camera.setScaleMode(scene::Camera::ScaleMode::showAll);
        camera.setTargetContentSize(Size<float, 2>{600.0F, 600.0F});
        cameraActor.addComponent(camera);
        layer.addChild(cameraActor);
        addLayer(layer);

        layer.addChild(menu);

        statusLabel.setPosition(Vector<float, 2>{0.0F, 220.0F});
        layer.addChild(statusLabel);

        smokeTestButton.setPosition(Vector<float, 2>{0.0F, 140.0F});
        menu.addWidget(smokeTestButton);

        textRefButton.setPosition(Vector<float, 2>{0.0F, 90.0F});
        menu.addWidget(textRefButton);

        demoButton.setPosition(Vector<float, 2>{0.0F, 40.0F});
        menu.addWidget(demoButton);

        enableImGuiButton.setPosition(Vector<float, 2>{0.0F, -30.0F});
        menu.addWidget(enableImGuiButton);

        backButton.setPosition(Vector<float, 2>{0.0F, -200.0F});
        menu.addWidget(backButton);
    }

    void ImGuiSample::refreshLabels()
    {
        const auto& host = imguiHost();
        smokeTestButton.getLabelDrawable()->setText(buttonLabel("Smoke test", host.showSmokeTest.load()));
        textRefButton.getLabelDrawable()->setText(buttonLabel("Text reference", host.showTextRef.load()));
        demoButton.getLabelDrawable()->setText(buttonLabel("Demo window", host.showDemo.load()));
        if (host.context)
            enableImGuiButton.getLabelDrawable()->setText(host.context->isEnabled()
                                                            ? "Disable ImGui"
                                                            : "Enable ImGui");
    }
}
