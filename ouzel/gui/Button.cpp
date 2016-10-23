// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <functional>
#include "Button.h"
#include "core/Engine.h"
#include "scene/Sprite.h"
#include "events/EventHandler.h"
#include "events/EventDispatcher.h"
#include "scene/Layer.h"
#include "scene/Camera.h"
#include "Label.h"
#include "utils/Utils.h"
#include "scene/TextDrawable.h"

namespace ouzel
{
    namespace gui
    {
        Button::Button(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage,
                       const std::string& label, const graphics::Color& labelColor, const std::string& font):
            eventHandler(EventHandler::PRIORITY_MAX + 1)
        {
            eventHandler.uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1, std::placeholders::_2);
            sharedEngine->getEventDispatcher()->addEventHandler(&eventHandler);

            if (!normalImage.empty())
            {
                normalSprite.reset(new scene::Sprite());
                if (normalSprite->initFromFile(normalImage, false))
                {
                    addComponent(normalSprite.get());
                }
            }

            if (!selectedImage.empty())
            {
                selectedSprite.reset(new scene::Sprite());
                if (selectedSprite->initFromFile(selectedImage, false))
                {
                    addComponent(selectedSprite.get());
                }
            }

            if (!pressedImage.empty())
            {
                pressedSprite.reset(new scene::Sprite());
                if (pressedSprite->initFromFile(pressedImage, false))
                {
                    addComponent(pressedSprite.get());
                }
            }

            if (!disabledImage.empty())
            {
                disabledSprite.reset(new scene::Sprite());
                if (disabledSprite->initFromFile(disabledImage, false))
                {
                    addComponent(disabledSprite.get());
                }
            }

            if (!label.empty())
            {
                labelDrawable.reset(new scene::TextDrawable(font, label));
                if (labelDrawable)
                {
                    labelDrawable->setColor(labelColor);
                    addComponent(labelDrawable.get());
                }
            }

            pickable = true;

            updateSprite();
        }

        void Button::setEnabled(bool newEnabled)
        {
            Widget::setEnabled(newEnabled);

            selected = false;
            pointerOver = false;
            pressed = false;

            updateSprite();
        }

        void Button::setSelected(bool newSelected)
        {
            Widget::setSelected(newSelected);

            updateSprite();
        }

        bool Button::handleUI(Event::Type type, const UIEvent& event)
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
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
            }

            return true;
        }

        void Button::updateSprite()
        {
            if (normalSprite) normalSprite->setHidden(true);
            if (selectedSprite) selectedSprite->setHidden(true);
            if (pressedSprite) pressedSprite->setHidden(true);
            if (disabledSprite) disabledSprite->setHidden(true);

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
        }
    } // namespace gui
} // namespace ouzel
