// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <functional>
#include "Button.hpp"
#include "core/Engine.hpp"
#include "events/EventHandler.hpp"
#include "events/EventDispatcher.hpp"
#include "scene/Layer.hpp"
#include "scene/Camera.hpp"
#include "Label.hpp"

namespace ouzel
{
    namespace gui
    {
        Button::Button()
        {
            eventHandler.uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1);
            engine->getEventDispatcher().addEventHandler(&eventHandler);

            pickable = true;
        }

        Button::Button(const std::string& normalImage,
                       const std::string& selectedImage,
                       const std::string& pressedImage,
                       const std::string& disabledImage,
                       const std::string& label,
                       const std::string& font,
                       float fontSize,
                       Color initLabelColor,
                       Color initLabelSelectedColor,
                       Color initLabelPressedColor,
                       Color initLabelDisabledColor):
            eventHandler(EventHandler::PRIORITY_MAX + 1),
            labelColor(initLabelColor),
            labelSelectedColor(initLabelSelectedColor),
            labelPressedColor(initLabelPressedColor),
            labelDisabledColor(initLabelDisabledColor)
        {
            eventHandler.uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1);
            engine->getEventDispatcher().addEventHandler(&eventHandler);

            if (!normalImage.empty())
            {
                normalSprite.reset(new scene::Sprite());
                normalSprite->init(normalImage);
                addComponent(normalSprite.get());
            }

            if (!selectedImage.empty())
            {
                selectedSprite.reset(new scene::Sprite());
                selectedSprite->init(selectedImage);
                addComponent(selectedSprite.get());
            }

            if (!pressedImage.empty())
            {
                pressedSprite.reset(new scene::Sprite());
                pressedSprite->init(pressedImage);
                addComponent(pressedSprite.get());
            }

            if (!disabledImage.empty())
            {
                disabledSprite.reset(new scene::Sprite());
                disabledSprite->init(disabledImage);
                addComponent(disabledSprite.get());
            }

            if (!label.empty())
            {
                labelDrawable.reset(new scene::TextRenderer(font, fontSize, label));
                labelDrawable->setColor(labelColor);
                addComponent(labelDrawable.get());
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

        bool Button::handleUI(const UIEvent& event)
        {
            if (!enabled) return false;

            if (event.actor == this)
            {
                if (event.type == Event::Type::ACTOR_ENTER)
                {
                    pointerOver = true;
                    updateSprite();
                }
                else if (event.type == Event::Type::ACTOR_LEAVE)
                {
                    pointerOver = false;
                    updateSprite();
                }
                else if (event.type == Event::Type::ACTOR_PRESS)
                {
                    pressed = true;
                    updateSprite();
                }
                else if (event.type == Event::Type::ACTOR_RELEASE ||
                         event.type == Event::Type::ACTOR_CLICK)
                {
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
            }

            return false;
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
                    pressedSprite->setHidden(false);
                else if (selected && selectedSprite)
                    selectedSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);

                if (labelDrawable)
                {
                    if (pressed && pointerOver)
                        labelDrawable->setColor(labelPressedColor);
                    else if (selected)
                        labelDrawable->setColor(labelSelectedColor);
                    else
                        labelDrawable->setColor(labelColor);
                }
            }
            else // disabled
            {
                if (disabledSprite)
                    disabledSprite->setHidden(false);
                else if (normalSprite)
                    normalSprite->setHidden(false);

                if (labelDrawable)
                    labelDrawable->setColor(labelDisabledColor);
            }
        }
    } // namespace gui
} // namespace ouzel
