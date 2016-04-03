// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CheckBox.h"
#include "Engine.h"

namespace ouzel
{
    namespace gui
    {
        CheckBox::CheckBox()
        {

        }

        CheckBox::~CheckBox()
        {
            sharedEngine->getEventDispatcher()->removeEventHandler(_eventHandler);
        }

        bool CheckBox::init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled, const std::string& tick)
        {
            _eventHandler = std::make_shared<EventHandler>(EventHandler::PRIORITY_MAX + 1);

            _eventHandler->gamepadHandler = std::bind(&CheckBox::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
            _eventHandler->uiHandler = std::bind(&CheckBox::handleUI, this, std::placeholders::_1, std::placeholders::_2);

            sharedEngine->getEventDispatcher()->addEventHandler(_eventHandler);

            return true;
        }

        void CheckBox::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);
        }

        void CheckBox::setChecked(bool checked)
        {
            _checked = checked;
        }

        bool CheckBox::handleGamepad(const GamepadEventPtr& event, const VoidPtr& sender)
        {
            return true;
        }

        bool CheckBox::handleUI(const UIEventPtr& event, const VoidPtr& sender)
        {
            return true;
        }

    } // namespace gui
} // namespace ouzel
