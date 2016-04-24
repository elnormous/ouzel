// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include "Types.h"
#include "Widget.h"
#include "Event.h"
#include "Color.h"

namespace ouzel
{
    class EventHandler;

    namespace gui
    {
        class Button: public Widget
        {
        public:
            static std::shared_ptr<Button> create(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled,
                                                  const std::string& label = "", const graphics::Color& labelColor = graphics::Color(255, 255, 255, 255), const std::string& font = "");

            Button();
            virtual ~Button();

            virtual bool init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled,
                              const std::string& label = "", const graphics::Color& labelColor = graphics::Color(255, 255, 255, 255), const std::string& font = "");

            virtual void setEnabled(bool enabled) override;

        protected:
            bool handleGamepad(const GamepadEventPtr& event, const VoidPtr& sender);
            bool handleUI(const UIEventPtr& event, const VoidPtr& sender);

            void updateSprite();

            scene::SpritePtr _normalSprite;
            scene::SpritePtr _selectedSprite;
            scene::SpritePtr _pressedSprite;
            scene::SpritePtr _disabledSprite;
            scene::TextDrawablePtr _label;

            EventHandlerPtr _eventHandler;

            bool _selected = false;
            bool _pointerOver = false;
            bool _pressed = false;
        };
    } // namespace gui
} // namespace ouzel
