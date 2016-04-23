// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <functional>
#include "Button.h"
#include "Engine.h"
#include "Sprite.h"
#include "EventHander.h"
#include "EventDispatcher.h"
#include "Layer.h"
#include "Camera.h"
#include "Label.h"
#include "Utils.h"

namespace ouzel
{
    namespace gui
    {
        std::shared_ptr<Button> Button::create(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled,
                                               const std::string& label, const graphics::Color& labelColor, const std::string& font)
        {
            std::shared_ptr<Button> result = std::make_shared<Button>();

            if (!result->init(normal, selected, pressed, disabled, label, labelColor, font))
            {
                result.reset();
            }

            return result;
        }

        Button::Button()
        {

        }

        Button::~Button()
        {
            sharedEngine->getEventDispatcher()->removeEventHandler(_eventHandler);
        }

        bool Button::init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled,
                          const std::string& label, const graphics::Color& labelColor, const std::string& font)
        {
            _eventHandler = std::make_shared<EventHandler>(EventHandler::PRIORITY_MAX + 1);

            _eventHandler->gamepadHandler = std::bind(&Button::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
            _eventHandler->uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1, std::placeholders::_2);

            sharedEngine->getEventDispatcher()->addEventHandler(_eventHandler);

            if (!normal.empty())
            {
                _normalSprite = std::make_shared<scene::Sprite>();
                if (_normalSprite->initFromFile(normal, false))
                {
                    _boundingBox = _normalSprite->getBoundingBox();
                    _normalSprite->setPickable(false);
                    addChild(_normalSprite);
                }
            }

            if (!selected.empty())
            {
                _selectedSprite = std::make_shared<scene::Sprite>();
                if (_selectedSprite->initFromFile(selected, false))
                {
                    _boundingBox.merge(_selectedSprite->getBoundingBox());
                    _selectedSprite->setPickable(false);
                    addChild(_selectedSprite);
                }
            }

            if (!pressed.empty())
            {
                _pressedSprite = std::make_shared<scene::Sprite>();
                if (_pressedSprite->initFromFile(pressed, false))
                {
                    _boundingBox.merge(_pressedSprite->getBoundingBox());
                    _pressedSprite->setPickable(false);
                    addChild(_pressedSprite);
                }
            }

            if (!disabled.empty())
            {
                _disabledSprite = std::make_shared<scene::Sprite>();
                if (_disabledSprite->initFromFile(disabled, false))
                {
                    _boundingBox.merge(_disabledSprite->getBoundingBox());
                    _disabledSprite->setPickable(false);
                    addChild(_disabledSprite);
                }
            }

            if (!label.empty())
            {
                _label = Label::create(font, label);
                if (_label)
                {
                    _boundingBox.merge(_label->getBoundingBox());
                    _label->setColor(labelColor);
                    _label->setPickable(false);
                    addChild(_label);
                }
            }

            updateSprite();

            return true;
        }

        void Button::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);

            _selected = false;
            _pointerOver = false;
            _pressed = false;
            _receiveInput = enabled;

            updateSprite();
        }

        bool Button::handleGamepad(const GamepadEventPtr& event, const VoidPtr& sender)
        {
            OUZEL_UNUSED(event);
            OUZEL_UNUSED(sender);

            return true;
        }

        bool Button::handleUI(const UIEventPtr& event, const VoidPtr& sender)
        {
            if (!_enabled) return true;

            if (sender.get() == this)
            {
                if (event->type == Event::Type::UI_ENTER_NODE)
                {
                    _pointerOver = true;
                    updateSprite();
                }
                else if (event->type == Event::Type::UI_LEAVE_NODE)
                {
                    _pointerOver = false;
                    updateSprite();
                }
                else if (event->type == Event::Type::UI_PRESS_NODE)
                {
                    _pressed = true;
                    updateSprite();
                }
                else if (event->type == Event::Type::UI_RELEASE_NODE)
                {
                    if (_pressed)
                    {
                        _pressed = false;
                        updateSprite();
                    }
                }
                else if (event->type == Event::Type::UI_CLICK_NODE)
                {
                    if (_pressed)
                    {
                        _pressed = false;
                        updateSprite();
                    }
                }
            }

            return true;
        }

        void Button::updateSprite()
        {
            if (_normalSprite) _normalSprite->setVisible(false);
            if (_selectedSprite) _selectedSprite->setVisible(false);
            if (_pressedSprite) _pressedSprite->setVisible(false);
            if (_disabledSprite) _disabledSprite->setVisible(false);

            if (_enabled)
            {
                if (_pressed && _pointerOver && _pressedSprite)
                {
                    _pressedSprite->setVisible(true);
                }
                else if (_selected && _selectedSprite)
                {
                    _selectedSprite->setVisible(true);
                }
                else if (_normalSprite)
                {
                    _normalSprite->setVisible(true);
                }
            }
            else
            {
                if (_disabledSprite)
                {
                    _disabledSprite->setVisible(true);
                }
                else if (_normalSprite)
                {
                    _normalSprite->setVisible(true);
                }
            }
        }
    } // namespace gui
} // namespace ouzel
