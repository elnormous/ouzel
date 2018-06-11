// Copyright (C) 2018 Elviss Strazdins
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
        CheckBox::CheckBox(const std::string& normalImage,
                           const std::string& selectedImage,
                           const std::string& pressedImage,
                           const std::string& disabledImage,
                           const std::string& tickImage):
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            eventHandler.uiHandler = std::bind(&CheckBox::handleUI, this, std::placeholders::_1, std::placeholders::_2);
            engine->getEventDispatcher()->addEventHandler(&eventHandler);

            if (!normalImage.empty())
            {
                normalSprite.reset(new scene::Sprite());
                if (normalSprite->init(normalImage, true))
                    addComponent(normalSprite.get());
            }

            if (!selectedImage.empty())
            {
                selectedSprite.reset(new scene::Sprite());
                if (selectedSprite->init(selectedImage, true))
                    addComponent(selectedSprite.get());
            }

            if (!pressedImage.empty())
            {
                pressedSprite.reset(new scene::Sprite());
                if (pressedSprite->init(pressedImage, true))
                    addComponent(pressedSprite.get());
            }

            if (!disabledImage.empty())
            {
                disabledSprite.reset(new scene::Sprite());
                if (disabledSprite->init(disabledImage, true))
                    addComponent(disabledSprite.get());
            }

            if (!tickImage.empty())
            {
                tickSprite.reset(new scene::Sprite());
                if (tickSprite->init(tickImage, true))
                    addComponent(tickSprite.get());
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

            if (event.actor == this)
            {
                if (type == Event::Type::ACTOR_ENTER)
                {
                    pointerOver = true;
                    updateSprite();
                }
                else if (type == Event::Type::ACTOR_LEAVE)
                {
                    pointerOver = false;
                    updateSprite();
                }
                else if (type == Event::Type::ACTOR_PRESS)
                {
                    pressed = true;
                    updateSprite();
                }
                else if (type == Event::Type::ACTOR_RELEASE)
                {
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
                else if (type == Event::Type::ACTOR_CLICK)
                {
                    pressed = false;
                    checked = !checked;
                    updateSprite();

                    Event changeEvent;
                    changeEvent.type = Event::Type::WIDGET_CHANGE;
                    changeEvent.uiEvent.actor = event.actor;
                    engine->getEventDispatcher()->postEvent(changeEvent);
                }
            }

            return true;
        }

        void CheckBox::updateSprite()
        {
            if (enabled)
            {
                if (pressed && pointerOver && pressedSprite)
                    pressedSprite->setHidden(false);
                else if (selected && selectedSprite)
                    selectedSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);
            }
            else
            {
                if (disabledSprite)
                    disabledSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);
            }

            if (tickSprite)
                tickSprite->setHidden(!checked);
        }
    } // namespace gui
} // namespace ouzel
