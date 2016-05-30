// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <functional>
#include "Button.h"
#include "core/Engine.h"
#include "scene/Sprite.h"
#include "events/EventHander.h"
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
        ButtonPtr Button::create(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage,
                                               const std::string& label, const graphics::Color& labelColor, const std::string& font)
        {
            ButtonPtr result = std::make_shared<Button>();
            result->init(normalImage, selectedImage, pressedImage, disabledImage, label, labelColor, font);

            return result;
        }

        Button::Button()
        {

        }

        Button::~Button()
        {
            sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
        }

        bool Button::init(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage,
                          const std::string& label, const graphics::Color& labelColor, const std::string& font)
        {
            eventHandler = std::make_shared<EventHandler>(EventHandler::PRIORITY_MAX + 1);

            eventHandler->gamepadHandler = std::bind(&Button::handleGamepad, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            eventHandler->uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

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

            if (!label.empty())
            {
                labelDrawable = scene::TextDrawable::create(font, label);
                if (labelDrawable)
                {
                    labelDrawable->setColor(labelColor);
                    addDrawable(labelDrawable);
                }
            }

            updateSprite();

            return true;
        }

        void Button::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);

            selected = false;
            pointerOver = false;
            pressed = false;
            receiveInput = enabled;

            updateSprite();
        }

        bool Button::handleGamepad(Event::Type type, const GamepadEvent& event, const VoidPtr& sender)
        {
            OUZEL_UNUSED(type);
            OUZEL_UNUSED(event);
            OUZEL_UNUSED(sender);

            return true;
        }

        bool Button::handleUI(Event::Type type, const UIEvent& event, const VoidPtr& sender)
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
            if (normalSprite) normalSprite->setVisible(false);
            if (selectedSprite) selectedSprite->setVisible(false);
            if (pressedSprite) pressedSprite->setVisible(false);
            if (disabledSprite) disabledSprite->setVisible(false);

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
        }
    } // namespace gui
} // namespace ouzel
