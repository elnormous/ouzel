// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CheckBox.h"
#include "Engine.h"
#include "Sprite.h"

namespace ouzel
{
    namespace gui
    {
        CheckBoxPtr CheckBox::create(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage)
        {
            CheckBoxPtr result = std::make_shared<CheckBox>();

            if (!result->init(normalImage, selectedImage, pressedImage, disabledImage, tickImage))
            {
                result.reset();
            }

            return result;
        }

        CheckBox::CheckBox()
        {

        }

        CheckBox::~CheckBox()
        {
            sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
        }

        bool CheckBox::init(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage)
        {
            eventHandler = std::make_shared<EventHandler>(EventHandler::PRIORITY_MAX + 1);

            eventHandler->gamepadHandler = std::bind(&CheckBox::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler->uiHandler = std::bind(&CheckBox::handleUI, this, std::placeholders::_1, std::placeholders::_2);

            sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

            if (!normalImage.empty())
            {
                normalSprite = std::make_shared<scene::Sprite>();
                if (normalSprite->initFromFile(normalImage, false))
                {
                    addDrawable(normalSprite);
                }
            }

            if (!selectedImage.empty())
            {
                selectedSprite = std::make_shared<scene::Sprite>();
                if (selectedSprite->initFromFile(selectedImage, false))
                {
                    addDrawable(selectedSprite);
                }
            }

            if (!pressedImage.empty())
            {
                pressedSprite = std::make_shared<scene::Sprite>();
                if (pressedSprite->initFromFile(pressedImage, false))
                {
                    addDrawable(pressedSprite);
                }
            }

            if (!disabledImage.empty())
            {
                disabledSprite = std::make_shared<scene::Sprite>();
                if (disabledSprite->initFromFile(disabledImage, false))
                {
                    addDrawable(disabledSprite);
                }
            }

            if (!tickImage.empty())
            {
                tickSprite = std::make_shared<scene::Sprite>();
                if (tickSprite->initFromFile(tickImage, false))
                {
                    addDrawable(tickSprite);
                }
            }

            return true;
        }

        void CheckBox::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);
        }

        void CheckBox::setChecked(bool newChecked)
        {
            checked = newChecked;
        }

        bool CheckBox::handleGamepad(const GamepadEventPtr& event, const VoidPtr& sender)
        {
            return true;
        }

        bool CheckBox::handleUI(const UIEventPtr& event, const VoidPtr& sender)
        {
            return true;
        }

        void CheckBox::updateSprite()
        {
        }
    } // namespace gui
} // namespace ouzel
