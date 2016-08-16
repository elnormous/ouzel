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
        CheckBox::CheckBox():
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
        }

        CheckBox::CheckBox(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage):
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            init(normalImage, selectedImage, pressedImage, disabledImage, tickImage);
        }

        CheckBox::~CheckBox()
        {
            sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
        }

        bool CheckBox::init(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage)
        {
            eventHandler.gamepadHandler = std::bind(&CheckBox::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler.uiHandler = std::bind(&CheckBox::handleUI, this, std::placeholders::_1, std::placeholders::_2);

            sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

            if (!normalImage.empty())
            {
                normalSprite = std::make_shared<scene::Sprite>();
                if (normalSprite->initFromFile(normalImage, false))
                {
                    addComponent(normalSprite);
                }
            }

            if (!selectedImage.empty())
            {
                selectedSprite = std::make_shared<scene::Sprite>();
                if (selectedSprite->initFromFile(selectedImage, false))
                {
                    addComponent(selectedSprite);
                }
            }

            if (!pressedImage.empty())
            {
                pressedSprite = std::make_shared<scene::Sprite>();
                if (pressedSprite->initFromFile(pressedImage, false))
                {
                    addComponent(pressedSprite);
                }
            }

            if (!disabledImage.empty())
            {
                disabledSprite = std::make_shared<scene::Sprite>();
                if (disabledSprite->initFromFile(disabledImage, false))
                {
                    addComponent(disabledSprite);
                }
            }

            if (!tickImage.empty())
            {
                tickSprite = std::make_shared<scene::Sprite>();
                if (tickSprite->initFromFile(tickImage, false))
                {
                    addComponent(tickSprite);
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

        bool CheckBox::handleGamepad(Event::Type, const GamepadEvent&)
        {
            return true;
        }

        bool CheckBox::handleUI(Event::Type type, const UIEvent& event)
        {
            if (!enabled) return true;

            if (event.node.get() == this)
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

                    Event changeEvent;
                    changeEvent.type = Event::Type::UI_WIDGET_CHANGE;
                    changeEvent.uiEvent.node = std::static_pointer_cast<Node>(shared_from_this());
                    sharedEngine->getEventDispatcher()->dispatchEvent(changeEvent);
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
