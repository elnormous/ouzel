// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CheckBox.h"
#include "Widget.h"
#include "core/Engine.h"
#include "scene/Sprite.h"
#include "events/EventDispatcher.h"

namespace ouzel
{
    namespace gui
    {
        CheckBox::CheckBox(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage):
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            eventHandler.uiHandler = std::bind(&CheckBox::handleUI, this, std::placeholders::_1, std::placeholders::_2);
            sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

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

            pickable = true;

            updateSprite();
        }

        void CheckBox::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);

            selected = false;
            pointerOver = false;
            pressed = false;

            updateSprite();
        }

        void CheckBox::setChecked(bool newChecked)
        {
            checked = newChecked;

            updateSprite();
        }

        bool CheckBox::handleUI(Event::Type type, const UIEvent& event)
        {
            if (!enabled) return true;

            if (event.node == this)
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
                    changeEvent.uiEvent.node = event.node;
                    sharedEngine->getEventDispatcher()->postEvent(changeEvent);
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
                    pressedSprite->setHidden(false);
                }
                else if (selected && selectedSprite)
                {
                    selectedSprite->setHidden(false);
                }
                else if (normalSprite)
                {
                    normalSprite->setHidden(false);
                }
            }
            else
            {
                if (disabledSprite)
                {
                    disabledSprite->setHidden(false);
                }
                else if (normalSprite)
                {
                    normalSprite->setHidden(false);
                }
            }

            if (tickSprite)
            {
                tickSprite->setHidden(!checked);
            }
        }
    } // namespace gui
} // namespace ouzel
