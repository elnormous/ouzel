// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CheckBox.hpp"
#include "Widget.hpp"
#include "core/Engine.hpp"
#include "scene/Sprite.hpp"
#include "events/EventDispatcher.hpp"

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
                normalSprite.reset(new scene::Sprite());
                if (normalSprite->init(normalImage, false))
                {
                    addComponent(normalSprite.get());
                }
            }

            if (!selectedImage.empty())
            {
                selectedSprite.reset(new scene::Sprite());
                if (selectedSprite->init(selectedImage, false))
                {
                    addComponent(selectedSprite.get());
                }
            }

            if (!pressedImage.empty())
            {
                pressedSprite.reset(new scene::Sprite());
                if (pressedSprite->init(pressedImage, false))
                {
                    addComponent(pressedSprite.get());
                }
            }

            if (!disabledImage.empty())
            {
                disabledSprite.reset(new scene::Sprite());
                if (disabledSprite->init(disabledImage, false))
                {
                    addComponent(disabledSprite.get());
                }
            }

            if (!tickImage.empty())
            {
                tickSprite.reset(new scene::Sprite());
                if (tickSprite->init(tickImage, false))
                {
                    addComponent(tickSprite.get());
                }
            }

            pickable = true;

            updateSprite();
        }

        void CheckBox::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

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
                if (type == Event::Type::NODE_ENTER)
                {
                    pointerOver = true;
                    updateSprite();
                }
                else if (type == Event::Type::NODE_LEAVE)
                {
                    pointerOver = false;
                    updateSprite();
                }
                else if (type == Event::Type::NODE_PRESS)
                {
                    pressed = true;
                    updateSprite();
                }
                else if (type == Event::Type::NODE_RELEASE)
                {
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
                else if (type == Event::Type::NODE_CLICK)
                {
                    pressed = false;
                    checked = !checked;
                    updateSprite();

                    Event changeEvent;
                    changeEvent.type = Event::Type::WIDGET_CHANGE;
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
