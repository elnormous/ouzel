// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Application.h"
#include "MainMenu.h"

using namespace std;
using namespace ouzel;

Application::~Application()
{
}

void Application::begin()
{
    sharedEngine->getInput()->startGamepadDiscovery();

#if defined(OUZEL_PLATFORM_WINDOWS) || defined(OUZEL_PLATFORM_LINUX)
    sharedEngine->getFileSystem()->addResourcePath("Resources");
#endif

    sharedEngine->getRenderer()->setClearColor(graphics::Color(64, 0, 0));
    sharedEngine->getWindow()->setTitle("Sample");

    eventHandler = make_shared<EventHandler>();

    eventHandler->keyboardHandler = bind(&Application::handleKeyboard, this, placeholders::_1, placeholders::_2, placeholders::_3);

    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    mainMenu = make_shared<MainMenu>();
    sharedEngine->getSceneManager()->setScene(mainMenu);
}

bool Application::handleKeyboard(Event::Type type, const KeyboardEvent& event, const VoidPtr& sender) const
{
    OUZEL_UNUSED(sender);

    if (type == Event::Type::KEY_DOWN && event.key == ouzel::input::KeyboardKey::ESCAPE)
    {
        if (sharedEngine->getSceneManager()->getScene() != mainMenu)
        {
            sharedEngine->getInput()->setCursorVisible(true);
            sharedEngine->getSceneManager()->setScene(mainMenu);
        }
        else
        {
            sharedEngine->exit();
        }

        return false;
    }
    
    return true;
}
