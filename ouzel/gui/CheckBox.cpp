// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CheckBox.h"
#include "core/Engine.h"
#include "scene/Sprite.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace gui
    {
        CheckBox::CheckBox(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage)
        {
            init(normalImage, selectedImage, pressedImage, disabledImage, tickImage);
        }

        CheckBox::~CheckBox()
        {
            sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
        }

        bool CheckBox::init(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage)
        {
            eventHandler = std::make_shared<EventHandler>(EventHandler::PRIORITY_MAX + 1);

            eventHandler->gamepadHandler = std::bind(&CheckBox::handleGamepad, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            eventHandler->uiHandler = std::bind(&CheckBox::handleUI, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

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

            updateSprite();

            return true;
        }

        void CheckBox::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);

            selected = false;
            pointerOver = false;
            pressed = false;
            receiveInput = enabled;

            updateSprite();
        }

        void CheckBox::setChecked(bool newChecked)
        {
            checked = newChecked;

            updateSprite();
        }

        bool CheckBox::handleGamepad(Event::Type type, const GamepadEvent& event, const VoidPtr& sender)
        {
            OUZEL_UNUSED(type);
            OUZEL_UNUSED(event);
            OUZEL_UNUSED(sender);

            return true;
        }

        bool CheckBox::handleUI(Event::Type type, const UIEvent& event, const VoidPtr& sender)
        {
            OUZEL_UNUSED(event);

            if (!enabled) return true;

            if (sender.get() == this)
            {
                if (type == Event::Type::UI_ENTER_NODE)
                {
                    pointerOver = true;
                    updateSprite();
                }
                else if (type == Event::Type::UI_LEAVE_NODE)
                {
                    pointerOver = false;
                    updateSprite();
                }
                else if (type == Event::Type::UI_PRESS_NODE)
                {
                    pressed = true;
                    updateSprite();
                }
                else if (type == Event::Type::UI_RELEASE_NODE)
                {
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
                else if (type == Event::Type::UI_CLICK_NODE)
                {
                    pressed = false;
                    checked = !checked;
                    updateSprite();

                    Event event;
                    event.sender = shared_from_this();
                    event.type = Event::Type::UI_WIDGET_CHANGE;
                    sharedEngine->getEventDispatcher()->dispatchEvent(event);
                }
            }

            return true;
        }

        void CheckBox::updateSprite()
        {
            if (enabled)
            {
                if (pressed && pointerOver && pressedSprite)
                {
                    pressedSprite->setVisible(true);
                }
                else if (selected && selectedSprite)
                {
                    selectedSprite->setVisible(true);
                }
                else if (normalSprite)
                {
                    normalSprite->setVisible(true);
                }
            }
            else
            {
                if (disabledSprite)
                {
                    disabledSprite->setVisible(true);
                }
                else if (normalSprite)
                {
                    normalSprite->setVisible(true);
                }
            }

            if (tickSprite)
            {
                tickSprite->setVisible(checked);
            }
        }
    } // namespace gui
} // namespace ouzel
